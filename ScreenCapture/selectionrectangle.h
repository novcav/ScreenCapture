//
// draw selection rectangle
//

#pragma once

#define SR_NONE                0
#define SR_UPPER_LEFT_CORNER   1
#define SR_UPPER_SIDE          2
#define SR_UPPER_RIGHT_CORNER  3
#define SR_RIGHT_SIDE          4
#define SR_LOWER_RIGHT_CORNER  5
#define SR_LOWER_SIDE          6
#define SR_LOWER_LEFT_CORNER   7
#define SR_LEFT_SIDE           8

class CSelectionRectangle
{
private:
	UINT loc;
	LPCTSTR cursor;
	bool dragging;
	int x1, y1, x2, y2;
	int ox, oy;

public:
	int x, y, width, height;

public:
	CSelectionRectangle();
	~CSelectionRectangle();

	void OnLButtonDown(HWND hWnd, int x, int y);
	void OnLButtonUp(HWND hWnd, int x, int y);
	bool OnMouseMove(HWND hWnd, int x, int y);

	void Draw(HDC hDC);

};

