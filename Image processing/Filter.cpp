#include "Filter.h"
#include <QImage>
#include <iostream>
#include <vector>
#include <cstdlib>

template <class T>
T clamp(T value, T max, T min)
{
	if (value > max)
		return max;
	if (value < min)
		return min;
	return value;
}

// ÌÀÒÅÌÀÒÈ×ÅÑÊÀß ÌÎÐÔÎËÎÃÈß

QImage Dilatation(const QImage& img, std::vector<std::vector<bool>> mask)
{
	QImage result(img);
	int MH = mask.size();
	int MW = mask.front().size();
	auto cmp{ [](QColor A, QColor B) -> bool {
		return (A.red() + A.green() + A.blue()) < (B.red() + B.green() + B.blue());
	} };
	for (int y = MH / 2; y < img.height() - MH / 2; ++y) {
		for (int x = MW / 2; x < img.width() - MW / 2; ++x) {
			QColor max_v{ 0, 0, 0 };
			for (int j = -MH / 2; j <= MH / 2; ++j) {
				for (int i = -MW / 2; i <= MW / 2; ++i) {
					if ((mask[MH / 2 + i][MW / 2 + j]) && (cmp(max_v, img.pixelColor(x + i, y + j)))) {
						max_v = img.pixelColor(x + i, y + j);
					}
				}
			}
			result.setPixelColor(x, y, max_v);
		}
	}
	return result;
}

QImage Erosion(const QImage& img, std::vector<std::vector<bool>> mask)
{
	QImage result(img);
	int MH = mask.size();
	int MW = mask.front().size();
	auto cmp{ [](QColor A, QColor B) -> bool {
		return (A.red() + A.green() + A.blue()) > (B.red() + B.green() + B.blue());
	} };
	for (int y = MH / 2; y < img.height() - MH / 2; ++y) {
		for (int x = MW / 2; x < img.width() - MW / 2; ++x) {
			QColor min_v{ 255, 255, 255 };
			for (int j = -MH / 2; j <= MH / 2; ++j) {
				for (int i = -MW / 2; i <= MW / 2; ++i) {
					if ((mask[MH / 2 + i][MW / 2 + j]) && (cmp(min_v, img.pixelColor(x + i, y + j)))) {
						min_v = img.pixelColor(x + i, y + j);
					}
				}
			}
			result.setPixelColor(x, y, min_v);
		}
	}
	return result;
}

QImage Open(const QImage& img, std::vector<std::vector<bool>> mask) {
	return Dilatation(Erosion(img, mask), mask);
}

QImage Close(const QImage& img, std::vector<std::vector<bool>> mask) {
	return Erosion(Dilatation(img, mask), mask);
}

QImage Grad(const QImage& img, std::vector<std::vector<bool>> mask) {
	auto Dilatated = Dilatation(img, mask);
	auto Erosed = Erosion(img, mask);
	QImage result(img);
	for (int j = 0; j < img.height(); ++j) {
		for (int i = 0; i < img.width(); ++i) {
			auto dpix = Dilatated.pixel(i, j);
			auto epix = Erosed.pixel(i, j);
			result.setPixelColor(i, j, {
				abs(qRed(dpix) - qRed(epix)),
				abs(qGreen(dpix) - qGreen(epix)),
				abs(qBlue(dpix) - qBlue(epix))
				});
		}
	}
	return result;
}

// ÎÏÈÑÀÍÈÅ ÔÓÍÊÖÈÉ ÁÀÇÎÂÎÃÎ ÊËÀÑÑÀ Filter

QImage Filter::process(const QImage& img) const
{
	QImage result(img);

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = calcNewPixelColor(img, x, y);
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}

// ÏÅÐÅÃÐÓÇÊÈ process

QImage SobelFilter::process(const QImage& img) const
{
	auto result = GrayScaleFilter().process(img);

	auto X = SobelXFilter().process(result);
	auto Y = SobelYFilter().process(result);
	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			auto colorX = qGreen(X.pixel(x, y));
			auto colorY = qGreen(Y.pixel(x, y));
			int resultpix = clamp((float)std::sqrt(colorX * colorX + colorY * colorY), 255.f, 0.f);
			result.setPixelColor(x, y, QColor(resultpix, resultpix, resultpix));
		}
	}
	return result;
}

QImage PrewittFilter::process(const QImage& img) const
{
	auto result = GrayScaleFilter().process(img);

	auto X = PrewittXFilter().process(result);
	auto Y = PrewittYFilter().process(result);
	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			auto colorX = qGreen(X.pixel(x, y));
			auto colorY = qGreen(Y.pixel(x, y));
			int resultpix = clamp((float)std::sqrt(colorX * colorX + colorY * colorY), 255.f, 0.f);
			result.setPixelColor(x, y, QColor(resultpix, resultpix, resultpix));
		}
	}
	return result;
}

QImage GrayWorld::process(const QImage& img) const
{
	QImage result(img);

	float R = RedAvg(img);
	float G = GreenAvg(img);
	float B = BlueAvg(img);
	float Avg = (R + G + B) / 3;

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = calcNewPixelColor(img, x, y);
			color.setRgb(clamp(color.red() * Avg / R, 255.f, 0.f), clamp(color.green() * Avg / G, 255.f, 0.f), clamp(color.blue() * Avg / B, 255.f, 0.f));
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}

QImage BaseColor::process(const QImage& img) const
{
	QImage result(img);
	float R, G, B;
	int x_src, y_src;
	std::cout << "Enter pixel coord" << std::endl;
	std::cin >> x_src >> y_src;
	std::cout << "Enter 3 numbers - R G B of base color" << std::endl;
	std::cin >> R >> G >> B;
	QColor color_src = img.pixelColor(x_src, y_src);
	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = calcNewPixelColor(img, x, y);
			color.setRgb(clamp(color.red() * R / color_src.red(), 255.f, 0.f), clamp(color.green() * G / color_src.green(), 255.f, 0.f), clamp(color.blue() * B / color_src.blue(), 255.f, 0.f));
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}

QImage Shift::process(const QImage& img) const
{
	QImage result(img);

	for (int x = 0; x < img.width() - 50; ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = calcNewPixelColor(img, x + 50, y);
			result.setPixelColor(x, y, color);
		}
	}
	for (int x = img.width() - 50; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color;
			color.setRgb(0, 0, 0);
			result.setPixelColor(x, y, color);
		}
	}
	return result;
}

QImage HistFilter::process(const QImage& img) const
{
	QImage result(img);
	int v_max = 0, v_min = 255;

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			auto pix = img.pixel(x, y);
			auto v = std::max({ qRed(pix), qBlue(pix), qGreen(pix) });
			v_max = std::max(v, v_max);
			v_min = std::min(v, v_min);
		}
	}
	auto stretch{ [v_max, v_min](int value) -> int {
			return ((value - v_min) * 255) / (v_max - v_min);
	} };
	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			auto pix = img.pixel(x, y);
			result.setPixelColor(x, y, { stretch(qRed(pix)), stretch(qGreen(pix)), stretch(qBlue(pix)) });
		}
	}

	return result;
}

// ÑÐÅÄÍÅÅ ÄËß GrayWorld

float Filter::RedAvg(const QImage& img) const
{
	float result = 0;
	int N = 0;

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = img.pixelColor(x, y);
			result += color.red();
			N++;
		}
	}
	return result / N;
}

float Filter::GreenAvg(const QImage& img) const
{
	float result = 0;
	int N = 0;

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = img.pixelColor(x, y);
			result += color.green();
			N++;
		}
	}
	return result / N;
}

float Filter::BlueAvg(const QImage& img) const
{
	float result = 0;
	int N = 0;

	for (int x = 0; x < img.width(); ++x) {
		for (int y = 0; y < img.height(); ++y) {
			QColor color = img.pixelColor(x, y);
			result += color.blue();
			N++;
		}
	}
	return result / N;
}

// ÏÅÐÅÃÐÓÇÊÈ calcNewPixelColor ÄËß ÒÎ×Å×ÍÛÕ ÔÈËÜÒÐÎÂ

QColor GrayWorld::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(color.red(), color.green(), color.blue());
	return color;
}

QColor BaseColor::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(color.red(), color.green(), color.blue());
	return color;
}

QColor InvertFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(255 - color.red(), 255 - color.green(), 255 - color.blue());
	return color;
}

QColor GrayScaleFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	float Intensity = 0.299 * color.red() + 0.587 * color.green() + 0.113 * color.blue();
	color.setRgb((int)Intensity, (int)Intensity, (int)Intensity);
	return color;
}

QColor Sepia::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	const int k = 15;
	float Intensity = 0.299 * color.red() + 0.587 * color.green() + 0.113 * color.blue();
	float returnR = Intensity + 2 * k;
	float returnG = Intensity + 0.5 * k;
	float returnB = Intensity - 1 * k;
	color.setRgb(clamp(returnR, 255.f, 0.f), clamp(returnG, 255.f, 0.f), clamp(returnB, 255.f, 0.f));
	return color;
}

QColor Brightness::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	const int k = 50;
	float returnR = color.red() + k;
	float returnG = color.green() + k;
	float returnB = color.blue() + k;
	color.setRgb(clamp(returnR, 255.f, 0.f), clamp(returnG, 255.f, 0.f), clamp(returnB, 255.f, 0.f));
	return color;
}

QColor Shift::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	color.setRgb(color.red(), color.green(), color.blue());
	return color;
}

QColor Glass_effect::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float new_x, new_y;
	new_x = (float(std::rand() % 100) / 100 - 0.5) * 10;
	new_y = (float(std::rand() % 100) / 100 - 0.5) * 10;
	//float temp = (float(std::rand() % 10) / 10 - 0.5) * 10;
	//std::cout << temp << std::endl;
	//new_x = x + (float(std::rand() % RAND_MAX) - 0.5) * 10;
	//new_y = y + (float(std::rand() % RAND_MAX) - 0.5) * 10;
	if (new_x + x >= 0 && new_x + x < img.width() && new_y + y >= 0 && new_y + y < img.height()) {
		QColor color = img.pixelColor(new_x + x, new_y + y);
		color.setRgb(color.red(), color.green(), color.blue());
		return color;
	}
	else {
		QColor color = img.pixelColor(x, y);
		color.setRgb(color.red(), color.green(), color.blue());
		return color;
	}
}

QColor MedianFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	QColor color = img.pixelColor(x, y);
	std::vector<QColor> neibs(25, QColor{ 0, 0, 0 });
	int size = 0;
	for (int i = -2; i <= 2; ++i) {
		for (int j = -2; j <= 2; ++j) {
			if (x + i < 0 || x + i >= img.width()) continue;
			if (y + j < 0 || y + j >= img.height()) continue;
			neibs[size++] = img.pixelColor(x + i, y + j);
		}
	}
	std::sort(neibs.begin(), neibs.begin() + size, [](QColor A, QColor B) {return (A.red() + A.green() + A.blue()) < (B.red() + B.green() + B.blue()); });
	return neibs[12];
}

// ÏÅÐÅÃÐÓÇÊÀ calcNewPixelColor ÄËß ÌÀÒÐÈ×ÍÎÃÎ ÔÈËÜÒÐÀ

QColor MatrixFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	float returnR = 0;
	float returnG = 0;
	float returnB = 0;
	int size = mKernel.getSize();
	int radius = mKernel.getRadius();
	for (int i = -radius; i <= radius; ++i) {
		for (int j = -radius; j <= radius; ++j) {
			int idx = (i + radius) * size + j + radius;

			QColor color = img.pixelColor(clamp(x + j, img.width() - 1, 0), clamp(y + i, img.height() - 1, 0));

			returnR += color.red() * mKernel[idx];
			returnG += color.green() * mKernel[idx];
			returnB += color.blue() * mKernel[idx];
		}
	}

	return QColor(clamp(returnR, 255.f, 0.f), clamp(returnG, 255.f, 0.f), clamp(returnB, 255.f, 0.f));
}

QColor HistFilter::calcNewPixelColor(const QImage& img, int x, int y) const
{
	return { 0, 0, 0 };
}