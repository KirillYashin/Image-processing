#include <QtCore/QCoreApplication>
#include <QImage>
#include <iostream>
#include <string>
#include "Filter.h"


void main(int argc, char* argv[])
{
    std::string s;

    QImage img;
    InvertFilter invert;
    BlurFilter blur;
    GrayScaleFilter gray;
    GaussianFilter gauss;
    SobelFilter sobel;
    Sepia sepia;
    Brightness bright;
    GrayWorld gray_world;
    Shift shift;
    Glass_effect glass;
    SharpnessFilter sharp;
    MoreSharpnessFilter more_sharp;
    PrewittFilter prewitt;
    MedianFilter median;
    HistFilter hist;
    BaseColor base;

    for (int i = 0; i < argc; ++i) {
        if (!strcmp(argv[i], "-p") && (i + 1 < argc)) {
            s = argv[i + 1];
        }
    }
    img.load(QString(s.c_str()));

    img.save(QString("Images\Source.png"));

    Dilatation(
        img,
        {
            {0, 1, 0},
            {1, 1, 1},
            {0, 1, 0}
        }
    ).save(QString("Images\Dilatation.png"));

    Erosion(
        img,
        {
            {0, 1, 0},
            {1, 1, 1},
            {0, 1, 0}
        }
    ).save(QString("Images\Erosion.png"));

    Open(
        img,
        {
            {0, 1, 0},
            {1, 1, 1},
            {0, 1, 0}
        }
    ).save(QString("Images\Open.png"));

    Close(
        img,
        {
            {0, 1, 0},
            {1, 1, 1},
            {0, 1, 0}
        }
    ).save(QString("Images\Close.png"));

    Grad(
        img,
        {
            {0, 1, 0},
            {1, 1, 1},
            {0, 1, 0}
        }
    ).save(QString("Images\Grad.png"));

    invert.process(img).save(QString("Images\Invert.png"));
    blur.process(img).save(QString("Images\Blur.png"));
    gray.process(img).save(QString("Images\GrayScale.png"));
    gauss.process(img).save(QString("Images\Gauss.png"));
    sepia.process(img).save(QString("Images\Sepia.png"));
    bright.process(img).save(QString("Images\Brightness.png"));
    gray_world.process(img).save(QString("Images\GrayWorld.png"));
    shift.process(img).save(QString("Images\Shift.png"));
    glass.process(img).save(QString("Images\Glass.png"));
    sobel.process(img).save(QString("Images\Sobel.png"));
    sharp.process(img).save(QString("Images\Sharp.png"));
    more_sharp.process(img).save(QString("Images\MoreSharp.png"));
    prewitt.process(img).save(QString("Images\Prewitt.png"));
    median.process(img).save(QString("Images\Median.png"));
    hist.process(img).save(QString("Images\Hist.png"));
    base.process(img).save(QString("Images\BaseColor.png"));
}
