﻿#pragma once

#include <QtGlobal>
#include <QImage>

class MainWindow;
class Document;
class BaseImage;

MainWindow* getGlobalWindow();

Document* getGlobalDocument();

BaseImage* getGlobalImage();

void repaintView();

bool copyByteToImage(uchar* byteImage, int width, int height, QImage* pImage);

// Spline function
float spline(float* x, float* y, int n, float* t, int m, float* z);