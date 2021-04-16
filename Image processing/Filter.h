#pragma once
#include <QImage>
#include <vector>

// Ã¿“≈Ã¿“»◊≈— ¿ﬂ ÃŒ–‘ŒÀŒ√»ﬂ

QImage Dilatation(const QImage& img, std::vector<std::vector<bool>> mask);
QImage Erosion(const QImage& img, std::vector<std::vector<bool>> mask);
QImage Open(const QImage& img, std::vector<std::vector<bool>> mask);
QImage Close(const QImage& img, std::vector<std::vector<bool>> mask);
QImage Grad(const QImage& img, std::vector<std::vector<bool>> mask);

class Filter
{
protected:
	virtual QColor calcNewPixelColor(const QImage& img, int x, int y) const = 0;
	float RedAvg(const QImage& img) const;
	float GreenAvg(const QImage& img) const;
	float BlueAvg(const QImage& img) const;
public:
	virtual ~Filter() = default;
	virtual QImage process(const QImage& img) const;
};

//“Œ◊≈◊Õ€≈ ‘»À‹“–€

class InvertFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class GrayScaleFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class Sepia : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class Brightness : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class GrayWorld : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	QImage process(const QImage& img) const override;
};

class BaseColor : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	QImage process(const QImage& img) const override;
};

class Shift : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	QImage process(const QImage& img) const override;
};

class Glass_effect : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class MedianFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
};

class HistFilter : public Filter
{
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	QImage process(const QImage& img) const override;
};

// ﬂƒ–¿

class Kernel
{
protected:
	std::unique_ptr<float[]> data;
	std::size_t radius;
	std::size_t getLen() const { return getSize() * getSize(); }
public:
	Kernel(std::size_t radius) : radius(radius)
	{
		data = std::make_unique<float[]>(getLen());
	}
	Kernel(const Kernel& other) : Kernel(other.radius)
	{
		std::copy(other.data.get(), other.data.get() + getLen(), data.get());
	}
	std::size_t getRadius() const { return radius; }
	std::size_t getSize() const { return 2 * radius + 1; }
	float operator[] (std::size_t id) const { return data[id]; }
	float& operator[] (std::size_t id) { return data[id]; }
};

// —Œ¡≈À‹

class SobelKernel_X : public Kernel
{
public:
	using Kernel::Kernel;
	SobelKernel_X() : Kernel(1)
	{
		static const float KERNELDATA[]{
			-1, -2, -1,
			0, 0, 0,
			1, 2, 1
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

class SobelKernel_Y : public Kernel
{
public:
	using Kernel::Kernel;
	SobelKernel_Y() : Kernel(1)
	{
		static const float KERNELDATA[]{
			-1, 0, 1,
			-2, 0, 2,
			-1, 0, 1
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

// œ–ﬁ»““

class PrewittKernel_X : public Kernel
{
public:
	using Kernel::Kernel;
	PrewittKernel_X() : Kernel(1)
	{
		static const float KERNELDATA[]{
			-1, 0, 1,
			-1, 0, 1,
			-1, 0, 1
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

class PrewittKernel_Y : public Kernel
{
public:
	using Kernel::Kernel;
	PrewittKernel_Y() : Kernel(1)
	{
		static const float KERNELDATA[]{
			-1, -1, -1,
			0, 0, 0,
			1, 1, 1
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

// –≈« Œ—“‹

class SharpnessKernel : public Kernel
{
public:
	using Kernel::Kernel;
	SharpnessKernel() : Kernel(1)
	{
		static const float KERNELDATA[]{
			0, -1, 0,
			-1, 5, -1,
			0, -1, 0
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

class MoreSharpnessKernel : public Kernel
{
public:
	using Kernel::Kernel;
	MoreSharpnessKernel() : Kernel(1)
	{
		static const float KERNELDATA[]{
			-1, -1, -1,
			-1, 9, -1,
			-1, -1, -1
		};
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = KERNELDATA[i];
	}
};

// ¡Àﬁ–

class BlurKernel : public Kernel
{
public:
	using Kernel::Kernel;
	BlurKernel(std::size_t radius = 2) : Kernel(radius)
	{
		for (std::size_t i = 0; i < getLen(); i++)
			data[i] = 1.0f / getLen();
	}
};

class GaussianKernel : public Kernel
{
public:
	using Kernel::Kernel;
	GaussianKernel(std::size_t radius = 2, float sigma = 3.f) : Kernel(radius)
	{
		float norm = 0;
		int signed_radius = static_cast<int>(radius);
		for (int x = -signed_radius; x <= signed_radius; ++x) {
			for (int y = -signed_radius; y <= signed_radius; ++y) {
				std::size_t idx = (x + radius) * getSize() + (y + radius);
				data[idx] = std::exp(-(x * x + y * y) / (sigma * sigma));
				norm += data[idx];
			}
		}

		for (std::size_t i = 0; i < getLen(); ++i) {
			data[i] /= norm;
		}
	}
};

// Ã¿“–»◊Õ€≈ ‘»À‹“–€

class MatrixFilter : public Filter
{
protected:
	Kernel mKernel;
	QColor calcNewPixelColor(const QImage& img, int x, int y) const override;
public:
	MatrixFilter(const Kernel& kernel) : mKernel(kernel) {};
	virtual ~MatrixFilter() = default;
};

// ¡Àﬁ–

class BlurFilter : public MatrixFilter
{
public:
	BlurFilter(std::size_t radius = 5) : MatrixFilter(BlurKernel(radius)) {}
};

class GaussianFilter : public MatrixFilter
{
public:
	GaussianFilter(std::size_t radius = 5, float sigma = 3.f) : MatrixFilter(GaussianKernel(radius, sigma)) {}
};

// —Œ¡≈À‹

class SobelXFilter : public MatrixFilter
{
public:
	SobelXFilter() : MatrixFilter(SobelKernel_X()) {}
};

class SobelYFilter : public MatrixFilter
{
public:
	SobelYFilter() : MatrixFilter(SobelKernel_Y()) {}
};

class SobelFilter : public MatrixFilter
{
public:
	SobelFilter() : MatrixFilter(Kernel(0)) {}
	QImage process(const QImage& img) const override;
};

//œ–ﬁ»““

class PrewittXFilter : public MatrixFilter
{
public:
	PrewittXFilter() : MatrixFilter(PrewittKernel_X()) {}
};

class PrewittYFilter : public MatrixFilter
{
public:
	PrewittYFilter() : MatrixFilter(PrewittKernel_Y()) {}
};

class PrewittFilter : public MatrixFilter
{
public:
	PrewittFilter() : MatrixFilter(Kernel(0)) {}
	QImage process(const QImage& img) const override;
};

//–≈« Œ—“‹

class SharpnessFilter : public MatrixFilter
{
public:
	SharpnessFilter() : MatrixFilter(SharpnessKernel()) {}
};

class MoreSharpnessFilter : public MatrixFilter
{
public:
	MoreSharpnessFilter() : MatrixFilter(SharpnessKernel()) {}
};

