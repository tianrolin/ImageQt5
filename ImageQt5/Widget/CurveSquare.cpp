#include "CurveSquare.h"

#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox>
#include <QtGlobal>
#include "../GlobalFunc.h"

CurveSquare::CurveSquare(QWidget* parent)
	: QWidget(parent)
	, _size(256)
	, _rectSquare(30, 0, _size, _size)
	, _curveOrLine(1)
	, _channel(0)
	, _activePegIndex(0)
{
	_heightArray = new uint[_size];
	memset(_heightArray, 0, sizeof(uint) * _size);

	_ownerPegs.append(Peg(0, _size));
	_ownerPegs.append(Peg(_size, 0));
	_activePegs = &_ownerPegs;

	// Initialize array
	_arrayIntensity = new uint[_size];
	_arrayRed = new uint[_size];
	_arrayGreen = new uint[_size];
	_arrayBlue = new uint[_size];
	for (int i = 0; i < _size; i++)
	{
		_arrayIntensity[i] = _arrayRed[i] = _arrayGreen[i] = _arrayBlue[i] = i;
	}
	_activeArray = _arrayIntensity;

	setMouseTracking(true);
}

CurveSquare::~CurveSquare()
{
	if (_heightArray)
	{
		delete[] _heightArray;
	}
	if (_arrayIntensity)
	{
		delete[] _arrayIntensity;
	}
	if (_arrayRed)
	{
		delete[] _arrayRed;
	}
	if (_arrayGreen)
	{
		delete[] _arrayGreen;
	}
	if (_arrayBlue)
	{
		delete[] _arrayBlue;
	}
}

void CurveSquare::init()
{
	BaseImage* image = getGlobalImage();

	// 根据活动通道更新绘制直方图
	switch (_channel)
	{
	case CURVE_CHANNEL_GRAY:
	{
	//	setActivePegArray(m_pegsIntensity);
	//	setActiveCurveArray(pWidget->GetIntensityArray());
	//	generateHistogram(image->getGrayPixelArray());	// 更新直方图
	}
	break;
	case CURVE_CHANNEL_RED:
	{
	//	setActivePegArray(m_pegsRed);
	//	setActiveCurveArray(pWidget->GetRedArray());
	//	generateHistogram(image->getRedPixelArray());
	}
	break;
	case CURVE_CHANNEL_GREEN:
	{
	//	setActivePegArray(m_pegsGreen);
	//	setActiveCurveArray(pWidget->GetGreenArray());
	//	generateHistogram(image->getGreenPixelArray());
	}
	break;
	case CURVE_CHANNEL_BLUE:
	{
	//	setActivePegArray(m_pegsBlue);
	//	setActiveCurveArray(pWidget->GetBlueArray());
	//	generateHistogram(image->getBluePixelArray());
	}
	break;
	}
}

void CurveSquare::reset()
{
	removeAllPegs();

	repaint();
}

QSize CurveSquare::sizeHint() const
{
	return QSize(256, 256);
}

QSize CurveSquare::minimumSizeHint() const
{
	return QSize(256, 256);
}

void CurveSquare::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	QSize size = event->size();

	int newSize = qMin(size.width(), size.height()) - 30;
	if (_size == newSize)
		return;

	_size = newSize;
	_rectSquare = QRect(30, 0, _size, _size);

	if (_heightArray)
	{
		delete[] _heightArray;
	}
	_heightArray = new uint[_size];
	memset(_heightArray, 0, sizeof(uint) * _size);

	_ownerPegs.clear();
	_ownerPegs.append(Peg(0, _size));
	_ownerPegs.append(Peg(_size, 0));
	_activePegs = &_ownerPegs;

	if (_arrayIntensity)
	{
		delete[] _arrayIntensity;
	}
	if (_arrayRed)
	{
		delete[] _arrayRed;
	}
	if (_arrayGreen)
	{
		delete[] _arrayGreen;
	}
	if (_arrayBlue)
	{
		delete[] _arrayBlue;
	}
	_arrayIntensity = new uint[_size];
	_arrayRed = new uint[_size];
	_arrayGreen = new uint[_size];
	_arrayBlue = new uint[_size];
	for (int i = 0; i < _size; i++)
	{
		_arrayIntensity[i] = _arrayRed[i] = _arrayGreen[i] = _arrayBlue[i] = i;
	}
	_activeArray = _arrayIntensity;

	repaint();

	// Emit signal to parent widget
	emit resize();
}

void CurveSquare::paintEvent(QPaintEvent* /*event*/)
{
	QPainter painter(this);

	paintHistogram();
	paintBackground();
}

void CurveSquare::paintHistogram()
{
	if (getGlobalDocument() == nullptr)
		return;

	if (_maxHeight == 0)
		return;
	
	QPainter painter(this);
	painter.setPen(QPen(Qt::blue));

	for (int i = 0; i < _size; i++)
	{
		painter.drawLine(i + _rectSquare.left(), _rectSquare.bottom(), i + _rectSquare.left(), _rectSquare.bottom() - _heightArray[i]);
	}
}

// Paint grid
void CurveSquare::paintBackground()
{
	QPainter painter(this);

	// Draw a horizontal ribbon and a vertical ribbon
	int gap = 6;
	QPoint point(_rectSquare.left() - gap, _rectSquare.top());
	int colorWidth = 15;
	for (int i = 0; i < _size; i++)
	{
		int j = 255 - i * 255 / (_size - 1);
		QPen pen(QColor(j, j, j));
		painter.setPen(pen);
		painter.drawLine(point.x() - colorWidth, point.y() + i, point.x(), point.y() + i);
	}

	point = QPoint(_rectSquare.left(), _rectSquare.bottom() + gap);
	for (int i = 0; i < _size; i++)
	{
		int j = i * 255 / (_size - 1);
		QPen pen(QColor(j, j, j));
		painter.setPen(pen);
		painter.drawLine(point.x() + i, point.y(), point.x() + i, point.y() + colorWidth);
	}

	// Draw square, three horizontal lines and three vertical lines
	QPen pen(Qt::black);
	painter.setPen(pen);
	painter.drawLine(_rectSquare.topLeft(), _rectSquare.topRight());
	painter.drawLine(_rectSquare.topRight(), _rectSquare.bottomRight());
	painter.drawLine(_rectSquare.bottomLeft(), _rectSquare.bottomRight());
	painter.drawLine(_rectSquare.topLeft(), _rectSquare.bottomLeft());
	
	pen.setStyle(Qt::DashLine);
	painter.setPen(pen);
	painter.drawLine(QPoint(_rectSquare.left(), _rectSquare.top() + _size / 4),
		QPoint(_rectSquare.right(), _rectSquare.top() + _size / 4));
	painter.drawLine(QPoint(_rectSquare.left(), _rectSquare.top() + _size / 2),
		QPoint(_rectSquare.right(), _rectSquare.top() + _size / 2));
	painter.drawLine(QPoint(_rectSquare.left(), _rectSquare.top() + _size * 3/ 4),
		QPoint(_rectSquare.right(), _rectSquare.top() + _size * 3 / 4));
	
	painter.drawLine(QPoint(_rectSquare.left() + _size / 4, _rectSquare.top()),
		QPoint(_rectSquare.left() + _size / 4, _rectSquare.bottom()));
	painter.drawLine(QPoint(_rectSquare.left() + _size / 2, _rectSquare.top()),
		QPoint(_rectSquare.left() + _size / 2, _rectSquare.bottom()));
	painter.drawLine(QPoint(_rectSquare.left() + _size * 3 / 4, _rectSquare.top()),
		QPoint(_rectSquare.left() + _size * 3 / 4, _rectSquare.bottom()));

	QColor color;
	switch (_channel)
	{
	case CURVE_CHANNEL_GRAY:
		color = QColor(0, 0, 0);
		break;
	case CURVE_CHANNEL_RED:
		color = QColor(255, 0, 0);
		break;
	case CURVE_CHANNEL_GREEN:
		color = QColor(0, 255, 0);
		break;
	case CURVE_CHANNEL_BLUE:
		color = QColor(0, 0, 255);
		break;
	default:
		color = QColor(0, 0, 0);
		break;
	}

	// Paint pegs
	paintPegs(color);

	// Paint connection
	paintConnection(color);
}

void CurveSquare::paintPegs(QColor color)
{
	// Traverse pegs array
	for (int i = 0; i < _activePegs->size(); i++)
	{
		paintSinglePeg(_activePegs->at(i), color);
	}

	// Paint active peg as a solid square
	if (_activePegIndex != NONE_PEG && _activePegIndex < _activePegs->size())
	{
		QPoint point = _activePegs->at(_activePegIndex).point(_rectSquare.topLeft());

		int size = 2;
		QRect rect(point.x() - size, point.y() - size, size * 2, size * 2);
		QPainter painter(this);
		painter.setBrush(QBrush(color));
		painter.drawRect(rect);
	}
}

// Paint single peg
void CurveSquare::paintSinglePeg(const Peg& peg, QColor color)
{
	QPoint point = peg.point(_rectSquare.topLeft());

	QPainter painter(this);
	painter.setPen(QPen(color));
	int size = 2;
	painter.drawLine(point.x() - size, point.y() - size, point.x() + size, point.y() - size);
	painter.drawLine(point.x() + size, point.y() - size, point.x() + size, point.y() + size);
	painter.drawLine(point.x() - size, point.y() - size, point.x() + size, point.y() + size);
}

// Paint connection
void CurveSquare::paintConnection(QColor color)
{
	if (_activeArray == nullptr)
		return;

	// Connect all pegs
	QVector<QLine> lines;
	for (int i = 0; i < _size - 1; i++)
	{
		lines.append(QLine(getCoordinate(i, _activeArray[i]), getCoordinate(i + 1, _activeArray[i + 1])));
	}

	QPainter painter(this);
	QPen pen(color);
	pen.setStyle(Qt::SolidLine);
	painter.setPen(pen);
	painter.drawLines(lines);
}

// 由x、y值计算出在控件客户区上的坐标
QPoint CurveSquare::getCoordinate(int x, int y)
{
	return QPoint(x + _rectSquare.left(), _size - y + _rectSquare.top());
}

void CurveSquare::mousePressEvent(QMouseEvent* event)
{
	QPoint point = event->pos();
	if (event->button() == Qt::LeftButton)
	{
		if (_rectSquare.contains(point))
		{
			QPoint ptDummy(point.x() - _rectSquare.left(), point.y() - _rectSquare.top());

			PegArray& pegs = *_activePegs;
		//	grabMouse();
			setCursor(Qt::SizeAllCursor);
			// nTemp用来纪录上一次的m_nActivePegIndex值
			int temp = _activePegIndex;
			_activePegIndex = ptInAnyPeg(ptDummy);
			if (_activePegIndex != NONE_PEG)
			{
				// 点落在已有的peg上,则刷新原m_nActivePegIndex所在的黑点
				if (temp != -1)
				{
					QRect rect(pegs[temp].x() - 2, pegs[temp].y() - 2, pegs[temp].x() + 2, pegs[temp].y() + 2);
					repaint(rect);
				}
			}
			else
			{
				// 点没落在任何peg上
				if (prepareAddPeg(ptDummy.x()))
				{
					// 增加新的peg，并刷新
					_activePegIndex = addPeg(ptDummy);
					// 改变数组m_Array值
					setArrayValue(_activePegIndex, true);
					repaintPeg();
					// 判断新插入的peg在temp位置前还是后
					if (temp >= _activePegIndex)
					{
						temp++;
					}
				}
				else
				{
				//	releaseMouse();
				}
				// 刷新旧的小方块
				if (temp != NONE_PEG && temp < pegs.size())
				{
					QRect rect(pegs[temp].x() - 2, pegs[temp].y() - 2, pegs[temp].x() + 2, pegs[temp].y() + 2);
					repaint(rect);
				}
			}
		}
	}
	else if(event->button() & Qt::RightButton)
	{
		PegArray& pegs = *_activePegs;
		if (_activePegIndex != NONE_PEG)
		{
			// 刷新原m_nActivePegIndex所在的黑点
			QRect rect(pegs[_activePegIndex].x() - 2, pegs[_activePegIndex].y() - 2,
				pegs[_activePegIndex].x() + 2, pegs[_activePegIndex].y() + 2);
			repaint(rect);
		}

		QPoint ptDummy(point.x() - _rectSquare.left(), point.y() - _rectSquare.top());
		_activePegIndex = ptInAnyPeg(ptDummy);
		// peg数目至少三个以上，才允许删除操作
		if (_activePegIndex != NONE_PEG && pegs.size() >= 3)
		{
			QPoint ptTemp = pegs[_activePegIndex].point();
			removePeg(_activePegIndex);
			setArrayValue(_activePegIndex, false);
			repaintPeg();
			if (_activePegIndex > pegs.size() - 1)
			{
				_activePegIndex--;
			}
		}
	}
}

void CurveSquare::mouseMoveEvent(QMouseEvent* event)
{
	QPoint point = event->pos();
	QPoint ptDummy(point.x() - _rectSquare.left(), point.y() - _rectSquare.top());

	if ((event->buttons() & Qt::LeftButton) == false)
	{
		// 之所以没有直接用m_rectSquare，而是把m_rectSquare.right+1&bottom+1，是为了能够使输入输出能达到255
		QRect rect(_rectSquare.left(), _rectSquare.top(), _rectSquare.width() + 1, _rectSquare.height() + 1);
		if (rect.contains(point))
		{
			// 显示鼠标所在点的输入输出值
		//	QSize size = getCurrentMouseValue(point);
		//	CString strTemp;
		//	strTemp.Format(_T("%d"), size.cx);
		//	GetDlgItem(ID_STATIC_CURVE_INPUT)->SetWindowText(strTemp);
		//	strTemp.Format(_T("%d"), size.cy);
		//	GetDlgItem(ID_STATIC_CURVE_OUTPUT)->SetWindowText(strTemp);

			// 鼠标左键没有按下时，仅改变光标形状
			if (ptInAnyPeg(ptDummy) != NONE_PEG)
			{
				setCursor(Qt::SizeAllCursor);
			}
			else
			{
				setCursor(Qt::ArrowCursor);
			}
		}
		else
		{
			// 鼠标移出m_rectSquare，则把静态控件置为空
		//	GetDlgItem(ID_STATIC_CURVE_INPUT)->SetWindowText(_T(""));
		//	GetDlgItem(ID_STATIC_CURVE_OUTPUT)->SetWindowText(_T(""));
		}
		return;
	}

	// 不允许把peg拉出m_rectSquare外
	if (!_rectSquare.contains(point))
		return;

	PegArray& pegs = *_activePegs;
	////////////////////////////// 处理peg移动事件 ////////////////////////////////
	// 特殊处理首尾两个peg
	if (_activePegIndex == 0)
	{
		if (ptDummy.x() < pegs[1].x() - PEG_DISTANCE)
		{
			QPoint ptTemp = pegs[0].point();
			pegs[0].setPoint(ptDummy);
			repaint();
			// 刷新StartPeg原来所在的小方形
			QRect rect(pegs[0].rect(_rectSquare.left(), _rectSquare.top()));
			repaint(rect);
			setArrayValue(_activePegIndex, true);
		}
	}
	else if (_activePegIndex == pegs.size() - 1)
	{
		if (ptDummy.x() > pegs[_activePegIndex - 1].x() + PEG_DISTANCE)
		{
			QPoint ptTemp = pegs[_activePegIndex].point();
			pegs[_activePegIndex].setPoint(ptDummy);
			repaintPeg();
			// 刷新EndPeg原来所在的小方形
			QRect rect(pegs[pegs.size() - 1].rect(_rectSquare.left(), _rectSquare.top()));
			repaint(rect);
			setArrayValue(_activePegIndex, true);
		}
	}
	else if (_activePegIndex != -1)
	{
		if ((ptDummy.x() > pegs[_activePegIndex - 1].x() + PEG_DISTANCE) &&
			(ptDummy.x() < pegs[_activePegIndex + 1].x() - PEG_DISTANCE))
		{
			int nFlag = 0;				// 标志是否发生peg合并事件

			QPoint ptTemp = pegs[_activePegIndex].point();
			pegs[_activePegIndex].setPoint(ptDummy);

			////////////////////////////// 处理合并peg事件 ////////////////////////////////
			// 与前面的peg合并
			if ((pegs[_activePegIndex].x() - pegs[_activePegIndex - 1].x() < 12) &&
				(abs(pegs[_activePegIndex].y() - pegs[_activePegIndex - 1].y()) < 12))
			{
				QPoint tempPoint = pegs[_activePegIndex].point();
				removePeg(_activePegIndex);
				nFlag = 1;
			}
			// 与后面的peg合并
			else if ((pegs[_activePegIndex + 1].x() - pegs[_activePegIndex].x() < 12) &&
				(abs(pegs[_activePegIndex + 1].y() - pegs[_activePegIndex].y()) < 12))
			{
				QPoint tempPoint = pegs[_activePegIndex].point();
				removePeg(_activePegIndex);
				nFlag = 2;
			}
			// 仅刷新与之相邻的两个peg所构成的最小矩形
			repaintPeg();

			setArrayValue(_activePegIndex, true);
			if (nFlag)
			{
				// nFlag==1对应减1, nFlag==2对应减0
				_activePegIndex -= (2 - nFlag);
			//	releaseMouse();
				return;
			}
		}
	}

	// 更新图像
//	if (!m_bIsProcessing)
//	{
//		AfxBeginThread(UpdateImageThread, this);
//	}
}

void CurveSquare::mouseReleaseEvent(QMouseEvent* event)
{
	setCursor(Qt::ArrowCursor);
	// 更新图像
//	AfxBeginThread(UpdateImageThread, this);

	emit update();
}

int CurveSquare::ptInAnyPeg(QPoint point) const
{
	for (int i = 0; i < _activePegs->size(); i++)
	{
		if (_activePegs->at(i).rect().contains(point))
		{
			return i;
		}
	}
	return NONE_PEG;
}

// 增加peg前的预处理
bool CurveSquare::prepareAddPeg(int xCoordinate)
{
	PegArray& pegs = *_activePegs;
	int i = 0, leftDis = 0, rightDis = 0;

	if (xCoordinate < pegs[0].x())
	{
		leftDis = pegs[0].x() - xCoordinate;
	}
	else
	{
		while (i < pegs.size() && pegs[i].x() < xCoordinate)
		{
			leftDis = xCoordinate - pegs[i].x();
			i++;
		}
	}

	if (i == pegs.size())
	{
		rightDis = xCoordinate - pegs[i - 1].x();
	}
	else
	{
		rightDis = pegs[i].x() - xCoordinate;
	}
	if (leftDis < PEG_DISTANCE || rightDis < PEG_DISTANCE)
	{
		return false;
	}

	return true;
}

// 增加peg
int CurveSquare::addPeg(const QPoint& point)
{
	Peg peg(point);
	// 插入链表
	_activePegs->append(peg);
	// 按照peg的横坐标大小排序
	return sortPegs(peg);
}

// Repaint
void CurveSquare::repaintPeg()
{
	QRect rect(_rectSquare);
	repaint(rect);
}

int CurveSquare::sortPegs(const Peg& peg)
{
	PegArray& pegs = *_activePegs;
	// 只有两个peg(首尾两个peg)时，不需排序
	if (pegs.size() == 2)
		return 0;

	// i从0开始循环的原因是有可能在首节点之前插入peg
	for (int i = 0; i < pegs.size() - 1; i++)
	{
		if (peg.x() < pegs[i].x())
		{
			// 刚插入的peg的横坐标不是最大，则进行排序
			for (int j = pegs.size() - 2; j > i - 1; j--)
			{
				pegs[j + 1] = pegs[j];
			}
			pegs[i] = peg;
			return i;
		}
	}
	return pegs.size() - 1;
}

void CurveSquare::setArrayValue(int index, bool flag)
{
	if (index == NONE_PEG)
		return;

//	UpdateData(TRUE);

	// 计算改变的区间内的新值
	if (_curveOrLine == 1)		// 直线的情况
	{
		if (index == -2)
		{
			// 此处为后续增加的代码, 仅为了代码重用
			for (int i = 0; i < _activePegs->size(); i++)
			{
				setLinearityArrayValue(i);
			}
		}
		else
		{
			setLinearityArrayValue(index, flag);
		}
	}
	else	// 曲线的情况
	{
	//	setCurveArrayValue();
	}
}

// 线性的改变数组的值
void CurveSquare::setLinearityArrayValue(int index, bool flag)
{
	if (index == 0)
	{
		QSize size = getCurrentValue(index);
		// 第一个peg之前的那些值置成与它的cy一样
		for (int i = 0; i <= size.width(); i++)
		{
			_activeArray[i] = size.height();
		}
		// 更新第一个peg与第二个peg之间的线段
		setLineValue(index);
	}
	else if (index > _activePegs->size() - 1 && !flag)
	{
		QSize size = getCurrentValue(index - 1);
		for (int i = _size - 1; i >= size.width(); i--)
		{
			_activeArray[i] = size.height();
		}
	}
	else if (index == _activePegs->size() - 1)
	{
		if (flag)
		{
			QSize size = getCurrentValue(index);
			for (int i = _size - 1; i >= size.width(); i--)
			{
				_activeArray[i] = size.height();
			}
			setLineValue(index - 1);
		}
		else
		{
			// 删除倒数第二个peg时的情况
			setLineValue(index - 1);
		}
	}
	else
	{
		if (flag)
		{
			setLineValue(index - 1);
			setLineValue(index);
		}
		else
		{
			// 删除peg的时候
			setLineValue(index - 1);
		}
	}
}

// 返回peg所在的输入输出值
QSize CurveSquare::getCurrentValue(int index)
{
	QPoint point(_activePegs->at(index).point());
	return QSize(point.x(), _size - point.y());
}

// 该函数只被setLinearityArrayValue()调用
void CurveSquare::setLineValue(int startIndex)
{
	if (startIndex < 0 || startIndex > _activePegs->size() - 2)
		return;

	QSize sizeStart = getCurrentValue(startIndex);
	QSize sizeEnd = getCurrentValue(startIndex + 1);

	for (int x = sizeStart.width(); x < sizeEnd.width(); x++)
	{
		_activeArray[x] = sizeStart.height() + (x - sizeStart.width()) * (sizeEnd.height() - sizeStart.height()) / (sizeEnd.width() - sizeStart.width());
	}
}

// Remove one peg
void CurveSquare::removePeg(int index)
{
	_activePegs->removeAt(index);
}

// Remove all pegs
void CurveSquare::removeAllPegs()
{
	_activePegs->clear();

	_activePegs->append(Peg(0, _size));
	_activePegs->append(Peg(_size, 0));
	_activePegIndex = 0;
	// Initialize array
	for (int i = 0; i < _size; i++)
	{
		_activeArray[i] = i;
	}
}

// Generate histogram
bool CurveSquare::generateHistogram(uint* pArray)
{
	for (int i = 0; i < _size - 1; i++)
	{
		float fIndex = i * 255.0f / float(_size - 1);
		int index = floor(fIndex);
		float fraction = fIndex - index;
		_heightArray[i] = (int)round(pArray[index] * (1.0f - fraction) + pArray[index + 1] * fraction);
	}
	_heightArray[_size - 1] = pArray[255];

	_minHeight = _maxHeight = _heightArray[0];
	// find max and min value in _heightArray
	for (int i = 1; i < _size; i++)
	{
		if (_maxHeight < _heightArray[i])
		{
			_maxHeight = _heightArray[i];
		}
		if (_minHeight > _heightArray[i])
		{
			_minHeight = _heightArray[i];
		}
	}
	// In case all pixels value are zero
	if (_minHeight == 0 && _maxHeight == 0)
		return false;

	// Calculate height
	for (int i = 0; i < _size; i++)
	{
		_heightArray[i] = (_heightArray[i] - _minHeight) * (_size - 1) / (_maxHeight - _minHeight) + 1;
	}

	repaint();

	return true;
}

void CurveSquare::setChannel(int channel)
{
	_channel = channel;
	repaint();

	// TODO
}