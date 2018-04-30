//
// draw selection rectangle
//

#include "stdafx.h"
#include "selectionrectangle.h"

// construction
CSelectionRectangle::CSelectionRectangle()
{
	dragging = false;

	width  = 682;
	height = 431;

	x = x1 = 39;
	y = y1 = 65;
	x2 = x1 + width;
	y2 = y1 + height;
}

// destruction
CSelectionRectangle::~CSelectionRectangle()
{
}

//
void CSelectionRectangle::OnLButtonDown(HWND hWnd, int x, int y)
{
	if(cursor == IDC_ARROW) return;

	ox = x - x1;
	oy = y - y1;

   SetCursor(LoadCursor(NULL, cursor));
   SetCapture(hWnd);
   dragging = true;
}

//
void CSelectionRectangle::OnLButtonUp(HWND hWnd, int x, int y)
{
	if(cursor == IDC_ARROW) return;

   ReleaseCapture();
   dragging = false;
}

//
bool CSelectionRectangle::OnMouseMove(HWND hWnd, int x, int y)
{
	static int px = 0;
	static int py = 0;
	int dx, dy;
	RECT rect;

	//       x1                   x2
	//    y1 +--------------------+
	//       |                    |
	//       |                    |
	//       |                    |
	//       |                    |
	//    y2 +--------------------+
	//

	if (dragging)
	{
		// resize the selection rectangle
		switch(loc)
		{
			case SR_UPPER_LEFT_CORNER:
				x1 = x;    y1 = y;
				break;
			case SR_LOWER_RIGHT_CORNER:
				x2 = x;    y2 = y;
				break;
			case SR_LOWER_LEFT_CORNER:
				x1 = x;    y2 = y;
				break;
			case SR_UPPER_RIGHT_CORNER:
				x2 = x;    y1 = y;
				break;
			case SR_LEFT_SIDE:
				x1 = x;
				break;
			case SR_RIGHT_SIDE:
				x2 = x;
				break;
			case SR_UPPER_SIDE:
				y1 = y;
				break;
			case SR_LOWER_SIDE:
				y2 = y;
				break;
			default:;
		}

		this->x = x1;
		this->y = y1;

		width  = x2 - x1;
		height = y2 - y1;

		dx = abs(x - px) + 5;
		dy = abs(y - py) + 5;

		SetRect(&rect, x1-dx, y1-dy, x2+dx, y2+dy);
		InvalidateRect(hWnd, &rect, TRUE);

		px = x;
		py = y;
	}
	else
	{
		// show mouse pointer on hover
		loc = SR_NONE;

		if((x1-2) < x && x < (x1+2) && (y1-2) < y && y < (y1+2))      loc = SR_UPPER_LEFT_CORNER;
		else if((x2-2) < x && x < (x2+2) && (y1-2) < y && y < (y1+2)) loc = SR_UPPER_RIGHT_CORNER;
		else if((x2-2) < x && x < (x2+2) && (y2-2) < y && y < (y2+2)) loc = SR_LOWER_RIGHT_CORNER;
		else if((x1-2) < x && x < (x1+2) && (y2-2) < y && y < (y2+2)) loc = SR_LOWER_LEFT_CORNER;

		else if(x1+10 < x && x < x2-10 && (y1-2) < y && y < (y1+2))   loc = SR_UPPER_SIDE;		
		else if(x1+10 < x && x < x2-10 &&  (y2-2) < y && y < (y2+2))  loc = SR_LOWER_SIDE;
		else if((x1-2) < x && x < (x1+2) && y1+10 < y && y < y2-10)   loc = SR_LEFT_SIDE;
		else if((x2-2) < x && x < (x2+2) && y1+10 < y && y < y2-10)   loc = SR_RIGHT_SIDE;

		switch(loc)
		{
			case SR_UPPER_LEFT_CORNER:;
			case SR_LOWER_RIGHT_CORNER:  cursor = IDC_SIZENWSE; break;
			case SR_LOWER_LEFT_CORNER:;
			case SR_UPPER_RIGHT_CORNER:  cursor = IDC_SIZENESW; break;
			case SR_LEFT_SIDE:;
			case SR_RIGHT_SIDE:          cursor = IDC_SIZEWE;   break;
			case SR_UPPER_SIDE:;
			case SR_LOWER_SIDE:          cursor = IDC_SIZENS;   break;
			default:                     cursor = IDC_ARROW; 
		}

		SetCursor(LoadCursor(NULL, cursor));
	}

	return dragging;
}

// draw rectangle
void CSelectionRectangle::Draw(HDC hDC)
{
	SelectObject(hDC, CreatePen(PS_SOLID, 3, RGB( 0, 0, 0)));
	Rectangle(hDC, x1, y1, x2, y2);
}
