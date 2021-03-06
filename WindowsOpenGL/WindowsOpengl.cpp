// WindowsOpengl.cpp : Define o ponto de entrada para o aplicativo.
//

#include "framework.h"
#include "stdafx.h"
#include "WindowsOpengl.h"
//#define GL_WIN_MAIN
//#include "GLfunctions.h" 
#include "Utils.h"
#include "..\..\ImagemEditorTRK3\Source\Imagem_003\Loaders.h"
#include "..\..\ImagemEditorTRK3\Source\Imagem_003\Imagem_003.h"

#pragma comment(lib, "../../ImagemEditorTRK3/Output/Win32_Debug/Imagem_003.lib")

GLContextStruct ctx;


int bQuit = false;
float theta = 0;
char limit = 1;

int nvert = 0, nindex = 0;
vec3* vert;
vec3* norm;
usvec3* index;

int tnvert = 0, tnindex = 0;
vec3* tvert;
vec3* tnorm;
usvec3* tindex;
pKeyMap keymap = new KeyMap;

animation anim;


void printAsciiTable() {
	FILE* ASSItable = fopen("ASSItable.py", "wb");
	char c = 0;
	for (char i = 0; i < 16; i++) {
		for (char j = 0; j < 16; j++) {
			fprintf(ASSItable, "%x: ''%c''			", c, c);
			c++;
		}
		fprintf(ASSItable, "\r\n");
	}
	fclose(ASSItable);

}

void decodeKeyPress(int key, WPARAM wParam, LPARAM lParam, pKeyMap keymap, UINT Message);

void decodeKeyPress2(pKeyMap keymap);
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient, lcxClient, lcyClient, iDeltaPerLine = 120, iAccumDelta;
	ULONG ulScrollLines = 1;
	static int xres, yres;
	PAINTSTRUCT ps;
	HDC hdc;
	RECT rc;
	ZeroMemory(keymap, sizeof(KeyMap));

	switch (Message)
	{
	case WM_CREATE:
	{
		printf("WM_CREATE\n");
		break;
	}
	case WM_NULL:
	{
		printf("WM_NULL\n");
	}
	case WM_SIZE:
	{
		GetWindowRect(hwnd, &rc);
		onResize(LOWORD(lParam), HIWORD(lParam), wParam, lParam);
		break;
	}
	case WM_MOVE:
	{
		onMove(LOWORD(lParam), HIWORD(lParam), wParam, lParam);
		break;
	}
	case WM_MOUSEMOVE:
	{
		onMouseMove(LOWORD(lParam), HIWORD(lParam), lParam, lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		onMousePress(LOWORD(lParam), HIWORD(lParam), lParam, lParam, false, 1);
		return 0;
	}
	case WM_MBUTTONDOWN:
	{
		onMousePress(LOWORD(lParam), HIWORD(lParam), lParam, lParam, false, 2);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		onMousePress(LOWORD(lParam), HIWORD(lParam), lParam, lParam, false, 3);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		onMousePress(LOWORD(lParam), HIWORD(lParam), lParam, lParam, true, 1);
		return 0;
	}
	case WM_MBUTTONUP:
	{
		onMousePress(LOWORD(lParam), HIWORD(lParam), lParam, lParam, true, 2);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		onMousePress(LOWORD(lParam), HIWORD(lParam), lParam, lParam, true, 3);
		return 0;
	}
	//case WM_KEYDOWN:
	//	if ((GetAsyncKeyState(VK_ESCAPE) & 0x01) && bRunning) {
	//		Stop();
	//	}
	//	break;
	//}
	case WM_KEYDOWN:
	{
		printf("WM_KEYDOWN\n");
		decodeKeyPress(wParam, wParam, lParam, keymap, Message);
		return 0;
	}
	case WM_KEYUP:
	{
		printf("WM_KEYUP\n");
		decodeKeyPress(wParam, wParam, lParam, keymap, Message);
		return 0;
	}
	//case WM_CHAR:
	//{
	//printf("WM_CHAR\n");
	//	pKeyMap keymap = new KeyMap;
	//	decodeKeyPress(wParam, wParam, lParam, keymap, Message);
	//	delete keymap;
	//	return 0;
	//}
	//case WM_DEADCHAR:
	//{
	//printf("WM_DEADCHAR\n");
	//	pKeyMap keymap = new KeyMap;
	//	decodeKeyPress(wParam, wParam, lParam, keymap, Message);
	//	delete keymap;
	//	return 0;
	//}
	case WM_SYSKEYDOWN:
	{
		printf("WM_SYSKEYDOWN\n");
		decodeKeyPress(wParam, wParam, lParam, keymap, Message);
		return 0;
	}
	case WM_SYSKEYUP:
	{
		printf("WM_SYSKEYUP\n");
		decodeKeyPress(wParam, wParam, lParam, keymap, Message);
		return 0;
	}
	case WM_SYSCHAR:
	{
		printf("WM_SYSCHAR\n");
		decodeKeyPress(wParam, wParam, lParam, keymap, Message);
		return 0;
	}
	case WM_SYSDEADCHAR:
	{
		printf("WM_SYSDEADCHAR\n");
		decodeKeyPress(wParam, wParam, lParam, keymap, Message);
		return 0;
	}
	case WM_MOUSEWHEEL:
	{
		if (iDeltaPerLine == 0)
			break;
		iAccumDelta += (short)HIWORD(wParam); // 120 or -120
		while (iAccumDelta >= iDeltaPerLine)
		{
			onMouseWhell(1, wParam, lParam);
			iAccumDelta -= iDeltaPerLine;
		}
		while (iAccumDelta <= -iDeltaPerLine)
		{
			onMouseWhell(-1, wParam, lParam);
			iAccumDelta += iDeltaPerLine;
		}
		break;
	}
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		onPaint(&ps, wParam, lParam);
		EndPaint(hwnd, &ps);
        SwapBuffers (ctx.hdc);
		return 0;
	}
	case WM_SETTINGCHANGE:
	{
		printf("WM_SETTINGCHANGE\n");
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);
		// ulScrollLines usually equals 3 or 0 (for no scrolling)
		// WHEEL_DELTA equals 120, so iDeltaPerLine will be 40
		if (ulScrollLines)
			iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
		else
			iDeltaPerLine = 0;
		break;
	}
	case WM_CLOSE:
	{
		printf("WM_CLOSE\n");
		DestroyWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		printf("WM_DESTROY\n");
		PostQuitMessage(0);
		break;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case 0:
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
		}
		break;
	}
	default:
		return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int startOpengl() {
	//mat4x4f h;
	//h.row0=0.0f;
	//h.aray[7]=8;
	//h.mt[1][1]=5;
	//h.x[1]=5;
	//h.
	//vec4 g[10];
	//g[1] = vec4(1.0f,2.0f,3.0f,4.0f);

	ctx.sx = 512;
	ctx.sy = 512;
	ZeroMemory(&ctx.wc, sizeof(WNDCLASS));
	ctx.wc.hInstance = GetModuleHandle(NULL);
	ctx.wc.lpfnWndProc = WndProc;
	ctx.wc.lpszClassName = L"TestOpengl_01";
	ctx.wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	ctx.wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.style = CS_HREDRAW | CS_VREDRAW;
	ctx.wc.hIcon = LoadIcon(NULL, IDI_ASTERISK);
	if (0 == RegisterClass(&ctx.wc)) return GL_TRUE;
	/* create window */
	ctx.hwnd = CreateWindow(L"TestOpengl_01", L"Test Opengl 01",
		WS_OVERLAPPEDWINDOW,
		111, 202, ctx.sx, ctx.sy,
		NULL, NULL, NULL, NULL);

	/* get the device context */
	ctx.hdc = GetDC(ctx.hwnd);
	// WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL

	/* find pixel format */

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int visual = ChoosePixelFormat(ctx.hdc, &pfd);
	/* set the pixel format for the dc */
	//SetPixelFormat(ctx.hdc, visual, &pfd);
	SetPixelFormat(ctx.hdc, 80, &pfd); // it is a hack, so i know that this number 80 activate the 4x AntiAlias on my computer
	/* create rendering context */
	ctx.hglrc = wglCreateContext(ctx.hdc);
	wglMakeCurrent(ctx.hdc, ctx.hglrc);
	LoadOGL();
	LoadWGL();

	printf(wglGetExtensionsStringARB(ctx.hdc));

	int attributes[] = {
	  WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	  WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	  WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
	  WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
	  WGL_COLOR_BITS_ARB, 32,
	  WGL_DEPTH_BITS_ARB, 24,
	  WGL_STENCIL_BITS_ARB, 8,
	  WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
	  WGL_SAMPLES_ARB, 4,        // Number of samples
	  0
	};

	int attrib[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
	int nResults[1] = { 0 };
	int pixFmt = 1;
	wglGetPixelFormatAttribivARB(ctx.hdc, pixFmt, 0, 1, attrib, nResults);


	GLint pfAttribCount[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
	//GLint pfAttribList[] = { 
	//	WGL_DRAW_TO_WINDOW_ARB, 
	//	WGL_ACCELERATION_ARB,
	//	WGL_SUPPORT_OPENGL_ARB, 
	//	WGL_DOUBLE_BUFFER_ARB, 
	//	WGL_DEPTH_BITS_ARB, 
	//	WGL_STENCIL_BITS_ARB, 
	//	WGL_RED_BITS_ARB, 
	//	WGL_GREEN_BITS_ARB, 
	//	WGL_BLUE_BITS_ARB, 
	//	WGL_ALPHA_BITS_ARB 
	// };
	GLint pfAttribList[] = {
		//WGL_NUMBER_PIXEL_FORMATS_ARB,
		//WGL_SUPPORT_OPENGL_ARB,
		//WGL_DOUBLE_BUFFER_ARB,
		//WGL_DRAW_TO_WINDOW_ARB,
		//WGL_DRAW_TO_BITMAP_ARB,
		//WGL_ACCELERATION_ARB,
		//WGL_NUMBER_OVERLAYS_ARB,
		//WGL_NUMBER_UNDERLAYS_ARB,
		//WGL_TRANSPARENT_ARB,
		//WGL_STEREO_ARB,
		//WGL_COLOR_BITS_ARB,
		//WGL_RED_BITS_ARB,
		//WGL_RED_SHIFT_ARB,
		//WGL_GREEN_BITS_ARB,
		//WGL_GREEN_SHIFT_ARB,
		//WGL_BLUE_BITS_ARB,
		//WGL_BLUE_SHIFT_ARB,
		//WGL_ALPHA_BITS_ARB,
		//WGL_ALPHA_SHIFT_ARB,
		//WGL_ACCUM_BITS_ARB,
		//WGL_ACCUM_RED_BITS_ARB,
		//WGL_ACCUM_GREEN_BITS_ARB,
		//WGL_ACCUM_BLUE_BITS_ARB,
		//WGL_ACCUM_ALPHA_BITS_ARB,
		//WGL_DEPTH_BITS_ARB,
		//WGL_STENCIL_BITS_ARB,
		//WGL_SHARE_DEPTH_ARB,
		//WGL_SHARE_STENCIL_ARB,
		//WGL_SHARE_ACCUM_ARB,
		//WGL_PIXEL_TYPE_ARB,
		//WGL_TRANSPARENT_RED_VALUE_ARB,
		//WGL_TRANSPARENT_GREEN_VALUE_ARB,
		//WGL_TRANSPARENT_BLUE_VALUE_ARB,
		//WGL_TRANSPARENT_ALPHA_VALUE_ARB,
		//WGL_TRANSPARENT_INDEX_VALUE_ARB,
		//WGL_NEED_PALETTE_ARB,
		//WGL_NEED_SYSTEM_PALETTE_ARB,
		//WGL_SWAP_LAYER_BUFFERS_ARB,
		//WGL_SWAP_METHOD_ARB,
		//WGL_SUPPORT_GDI_ARB,
		//WGL_AUX_BUFFERS_ARB,
		//WGL_SAMPLES_ARB

			WGL_SUPPORT_OPENGL_ARB,
			WGL_SUPPORT_GDI_ARB,
			WGL_DOUBLE_BUFFER_ARB,
			WGL_DRAW_TO_WINDOW_ARB,
			WGL_DRAW_TO_BITMAP_ARB,
			WGL_SWAP_METHOD_ARB,
			WGL_PIXEL_TYPE_ARB,
			WGL_AUX_BUFFERS_ARB,
			WGL_COLOR_BITS_ARB,
			WGL_DEPTH_BITS_ARB,
			WGL_STENCIL_BITS_ARB,
			WGL_RED_BITS_ARB,
			WGL_GREEN_BITS_ARB,
			WGL_BLUE_BITS_ARB,
			WGL_ALPHA_BITS_ARB,
			WGL_ACCUM_BITS_ARB,
			WGL_ACCUM_RED_BITS_ARB,
			WGL_ACCUM_GREEN_BITS_ARB,
			WGL_ACCUM_BLUE_BITS_ARB,
			WGL_ACCUM_ALPHA_BITS_ARB,
			WGL_SAMPLES_ARB,
	};

	FILE* LogPixelFormat = fopen("LogPixelFormat.py", "wb");



	int nPixelFormatCount = 0;
	wglGetPixelFormatAttribivARB(ctx.hdc, 1, 0, 1, pfAttribCount, &nPixelFormatCount);
	for (int i = 1; i < nPixelFormatCount; i++)
	{
		GLint results[21];
		fprintf(LogPixelFormat, "\nPixel format %d details:\n", i);
		wglGetPixelFormatAttribivARB(ctx.hdc, i, 0, 21, pfAttribList, results);
		//printf(" Draw to Window = %x:\n", results[0]); 
		//printf(" HW Accelerated = %x:\n", results[1]); 
		//printf(" Supports OpenGL = %x:\n", results[2]); 
		//printf(" Double Buffered = %x:\n", results[3]); 
		//printf(" Depth Bits = %x:\n", results[4]); 
		//printf(" Stencil Bits = %x:\n", results[5]); 
		//printf(" Red Bits = %x:\n", results[6]); 
		//printf(" Green Bits = %x:\n", results[7]); 
		//printf(" Blue Bits = %x:\n", results[8]); 
		//printf(" Alpha Bits = %x:\n", results[9]);

		//fprintf(LogPixelFormat, " WGL_NUMBER_PIXEL_FORMATS_ARB = %x:\n", results[0]);
		fprintf(LogPixelFormat, " WGL_SUPPORT_OPENGL_ARB = %d:\n", results[0]);
		fprintf(LogPixelFormat, " WGL_SUPPORT_GDI_ARB = %d:\n", results[1]);
		fprintf(LogPixelFormat, " WGL_DOUBLE_BUFFER_ARB = %d:\n", results[2]);
		fprintf(LogPixelFormat, " WGL_DRAW_TO_WINDOW_ARB = %d:\n", results[3]);
		fprintf(LogPixelFormat, " WGL_DRAW_TO_BITMAP_ARB = %d:\n", results[4]);
		fprintf(LogPixelFormat, " WGL_SWAP_METHOD_ARB = 0x%x:\n", results[5]);
		fprintf(LogPixelFormat, " WGL_PIXEL_TYPE_ARB = 0x%x:\n", results[6]);
		fprintf(LogPixelFormat, " WGL_SAMPLES_ARB = %d:\n", results[20]);
		fprintf(LogPixelFormat, " WGL_AUX_BUFFERS_ARB = %d:\n", results[7]);
		fprintf(LogPixelFormat, " WGL_COLOR_BITS_ARB = %d:\n", results[8]);
		fprintf(LogPixelFormat, " WGL_DEPTH_BITS_ARB = %d:\n", results[9]);
		fprintf(LogPixelFormat, " WGL_STENCIL_BITS_ARB = %d:\n", results[10]);
		fprintf(LogPixelFormat, " WGL_RED_BITS_ARB = %d:\n", results[11]);
		fprintf(LogPixelFormat, " WGL_GREEN_BITS_ARB = %d:\n", results[12]);
		fprintf(LogPixelFormat, " WGL_BLUE_BITS_ARB = %d:\n", results[13]);
		fprintf(LogPixelFormat, " WGL_ALPHA_BITS_ARB = %d:\n", results[14]);
		fprintf(LogPixelFormat, " WGL_ACCUM_BITS_ARB = %d:\n", results[15]);
		fprintf(LogPixelFormat, " WGL_ACCUM_RED_BITS_ARB = %d:\n", results[16]);
		fprintf(LogPixelFormat, " WGL_ACCUM_GREEN_BITS_ARB = %d:\n", results[17]);
		fprintf(LogPixelFormat, " WGL_ACCUM_BLUE_BITS_ARB = %d:\n", results[18]);
		fprintf(LogPixelFormat, " WGL_ACCUM_ALPHA_BITS_ARB = %d:\n", results[19]);
		//fprintf(LogPixelFormat, " WGL_RED_SHIFT_ARB = %d:\n", results[12]);
		//fprintf(LogPixelFormat, " WGL_GREEN_SHIFT_ARB = %d:\n", results[14]);
		//fprintf(LogPixelFormat, " WGL_BLUE_SHIFT_ARB = %d:\n", results[16]);
		//fprintf(LogPixelFormat, " WGL_ALPHA_SHIFT_ARB = %d:\n", results[18]);
		//fprintf(LogPixelFormat, " WGL_ACCELERATION_ARB = %x:\n", results[5]);
		//fprintf(LogPixelFormat, " WGL_NUMBER_OVERLAYS_ARB = %x:\n", results[6]);
		//fprintf(LogPixelFormat, " WGL_NUMBER_UNDERLAYS_ARB = %x:\n", results[7]);
		//fprintf(LogPixelFormat, " WGL_TRANSPARENT_ARB = %x:\n", results[8]);
		//fprintf(LogPixelFormat, " WGL_STEREO_ARB = %x:\n", results[9]);
		//fprintf(LogPixelFormat, " WGL_SHARE_DEPTH_ARB = %x:\n", results[26]);
		//fprintf(LogPixelFormat, " WGL_SHARE_STENCIL_ARB = %x:\n", results[27]);
		//fprintf(LogPixelFormat, " WGL_SHARE_ACCUM_ARB = %x:\n", results[28]);
		//fprintf(LogPixelFormat, " WGL_TRANSPARENT_RED_VALUE_ARB = %x:\n", results[30]);
		//fprintf(LogPixelFormat, " WGL_TRANSPARENT_GREEN_VALUE_ARB = %x:\n", results[31]);
		//fprintf(LogPixelFormat, " WGL_TRANSPARENT_BLUE_VALUE_ARB = %x:\n", results[32]);
		//fprintf(LogPixelFormat, " WGL_TRANSPARENT_ALPHA_VALUE_ARB = %x:\n", results[33]);
		//fprintf(LogPixelFormat, " WGL_TRANSPARENT_INDEX_VALUE_ARB = %x:\n", results[34]);
		//fprintf(LogPixelFormat, " WGL_NEED_PALETTE_ARB = %x:\n", results[35]);
		//fprintf(LogPixelFormat, " WGL_NEED_SYSTEM_PALETTE_ARB = %x:\n", results[36]);
		//fprintf(LogPixelFormat, " WGL_SWAP_LAYER_BUFFERS_ARB = %x:\n", results[37]);
	}
	fprintf(LogPixelFormat, "\n\n\n\n");


	fclose(LogPixelFormat);
	return 1;
}

objeto* esfera;
objeto* torus;
texto *txt;
	GLuint hdrTextures[10];

// Main function startup program
int main(int argc, char* argv[])
{
	FILE*  f = fopen("XING_T32.TGA", "rb");
	if(!f) f = fopen("../XING_T32.TGA", "rb");
	if(!f) f = fopen("../../XING_T32.TGA", "rb");
	//if(!f) f = fopen("../../concrete_diffuse.tga", "rb");
	img_basis* tx;
	tx = read_TGA(f);
	if (!tx->isOPenGLCompatible())
		tx->convertoToOPenGLCompatible();
	cTimer t;
	startOpengl();
	//glEnable(GL_MULTISAMPLE);
	glGenTextures(1, hdrTextures);

	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hdrTextures[0]);
	glBindTexture(GL_TEXTURE_2D, hdrTextures[0]);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB16F, ctx.sx, ctx.sy, GL_FALSE);
	glTexImage2D(GL_TEXTURE_2D, 0, tx->glInternalFormat, tx->xres, tx->yres, GL_FALSE, tx->glFormat, tx->glType, tx->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	//GLint buf, sbuf;
	//glClearColor(0.0, 0.0, 0.0, 0.0);
	//glGetIntegerv(GL_SAMPLE_BUFFERS, &buf);
	//glGetIntegerv(GL_SAMPLES, &sbuf);
	//printf("number of sample buffers is %i %i\n", buf, sbuf);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-1.0, 1.0, -1.0, 1.0, -2.0, 100.0);

	anim.frames.push_back(frame(vec2(0), vec2(0), vec2(0)));
	anim.frames.push_back(frame(vec2(0.5,0), vec2(0.501,1), vec2(1,1)));
	anim.frames.push_back(frame(vec2(1.5,1), vec2(1.6,0), vec2(2,1)));


	//shader s2;
	//printf("%i\n", s2.use());
	torus = new objeto(0, objType::objTorus, float3(0, 0, 0), float3(0, 0, 0), float3(1, 1, 1), uivec3(20, 20, 20));
	torus->atach();
	txt = new texto("abcdefghijklmnopqrstuvwxyz ,.;/\\[]{}??`=+-_()!????????????@#$%??&*'\"+-*/asdasdkhj	bfksdhgfgh	diasghfh hgfhjksdghfhj \ndhgadesfghadshfshg\nfsdfsd	asd\ndfsdfdsgdsgsdg)");
	//torus->makeTorus(20, 20, float3(1, 1, 1));
	//torus->CreateBuffer();
	esfera = new objeto(0, objType::objBox, float3(0, 0, 0), float3(0, 0, 0), float3(0.1, 0.1, 0.1), uivec3(20, 20, 20));
	esfera->atach();
	esfera->grot = vec4(0, 0, 0.00000001, 0.000000333);
	torus->draw();
	//esfera->makeSphere(20, 20, float3(1, 1, 1));
	//esfera->CreateBuffer();


	t.reset();
	ShowWindow(ctx.hwnd, 1);
	MSG msg;

	while (!bQuit)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			ZeroMemory(keymap, sizeof(KeyMap));
			decodeKeyPress2(keymap);
			//printf("time1: %f ms\n", setTime() / 1000.0f);
			onRenderScene();
			t.set();
			//printf("time2: %f ms\n", setTime() / 1000.0f);
			glFinish();
			t.set();
			//printf("time3: %f ms\n", t.getms());
			SwapBuffers(ctx.hdc);
			t.set();
			if (limit)
				while ((t.getms() ) <= 13.0f) 
					Sleep(6);
			t.set();
			//printf("time4: %f ms\n", t.getms());
			//printf("time4: %f fps\n\n", t.FPS());
			t.reset();
			//system("pause");
		}
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
	}

	delete keymap;
	delete[] vert;
	delete[] index;
	wglMakeCurrent(ctx.hdc, NULL);
	wglDeleteContext(ctx.hglrc);
	ReleaseDC(ctx.hwnd, ctx.hdc);
	DestroyWindow(ctx.hwnd);


	//GLint nNum; 
	//char * ext;

	//glGetIntegerv(GL_EXTENSIONS, &nNum);

	//for(GLint i = 1; i< 1000; i++){
	//	ext = (char *)glGetString(i);
	//	if(!ext)
	//		continue;
	//	printf(ext);
	// }
	// { 
	//wglSwapIntervalEXT = 
	//(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress(???wglSwapIntervalEXT???);
	//if(wglSwapIntervalEXT != NULL) 
	//wglSwapIntervalEXT(1);
	// }

	//auxInitDisplayMode(AUX_INDIRECT | AUX_RGBA);
	//auxInitPosition(100,100,250,250);
	//auxInitWindow("My first OpenGL Program");  
	//// These are the OpenGL functions that do something in the window
	//glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glFlush();
	//// Stop and wait for a keypress
	//printf("Press any key to close the Window \n");
	//getchar();

	//system("pause");
	//return msg.wParam;
	return 0;
}

void onRenderScene() {
	//torus->malhas[0]->mMaterial->mShader->readSrcFromFilenames();
	//torus->malhas[0]->mMaterial->mShader->generateShadersAndProgram();
	//esfera->malhas[0]->mMaterial->mShader->readSrcFromFilenames();
	//esfera->malhas[0]->mMaterial->mShader->generateShadersAndProgram();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_MULTISAMPLE_ARB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	//char a[] = "\r\n	 ";
	//printf("%x %x %x %x\n", a[0], a[1], a[2], a[3]);

	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, hdrTextures[0]);
	glLineWidth(1.0f);

	esfera->draw();
	//torus->draw();
	//torus->calcMatrix();
	//torus->malhas[0]->mMaterial->active(&torus->matrix);
	glUniform1i(glGetUniformLocation(esfera->malhas[0]->mMaterial->mShader->program, "texture1"), 0);
	//txt->draw();
	glPushMatrix();
	//glRotatef(theta, 0.5f, -1.5f, 0.0f);
	//glBegin (GL_TRIANGLES);
	//	glColor3f (1.0f, 0.0f, 0.0f);   glVertex3f (0.0f, 1.0f, 1.0f);
	//	glColor3f (0.0f, 1.0f, 0.0f);   glVertex3f (0.87f, -0.5f, -0.5f);
	//	glColor3f (0.0f, 0.0f, 1.0f);   glVertex3f (-0.87f, -0.5f, -0.5f);
	//glEnd ();
	//drawSphere(100, 100, vec3(0.75f, 0.75f, 0.75f));
	//drawCube(vec3(1.0f, 0.15f, 1.0f), 0x1|0x2|0x4);
	//shader s;
	//s.setSource(defaultFS, ShaderType::SHADER_FRAGMENT);
	//s.setSource(defaultVS, ShaderType::SHADER_VERTEX);
	//s.generateShadersAndProgram(1);
	//s.use();
	//drawRandPoints(1000);
	//drawTorus2(20, 11, vec3(0.5f, 0.5f, 0.5f));
	
	//drawText("abcdefghijklmnopqrstuvwxyz\nasdfasf\n", vec2(-1.f, 1.f), vec2(1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f), vec2(0.f, 0.f));
	//mat4 m = glm::rotate(mat4(1.0f), 0.01f, vec3(0, 1, 0));
	//vec4 q(activecamera->gpos, 0);
	//activecamera->gpos = vec3(m*q);
	//activecamera->fov = 0.500f;
	//activecamera->calcMatrix();


	glBegin (GL_LINE_STRIP);

	for (int i = 0; i < 200 * anim.frames[anim.frames.size()-1].p.x; i++) {
		glVertex3f (i/100.f, anim.getFrame(i/100.f), 0 );
	}
	glEnd ();

	activecamera->calcMatrix();
	matrix_block matrix;
	matrix.V = activecamera->matrix.V;
	matrix.P = activecamera->matrix.P;
	mat4 m = mat4(1.0f);
	m = glm::rotate(m, 0.f, vec3(0, 0, 0));
	m = glm::translate(m, vec3(0, 0, 0));
	//m = glm::scale(m, vec3(1, 1, 1));
	matrix.M = m;
	matrix.MV = matrix.V * matrix.M;
	matrix.MVP = matrix.P * matrix.V * matrix.M;
	material mMaterial("default.mat");
	////mMaterial->atach();
	mMaterial.mShader = new shader("default.vs", "VertexColor.fs");
	//mMaterial.mShader->FS = "VertexColor.fs";
	//mMaterial.atach();
	mMaterial.active(&matrix);



	//shader s("default.vs", "VertexColor.fs", 0,0,0);
	//s.use();
	drawGrid();
	drawAxis();

	glPopMatrix();
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);

	theta -= 0.1f;

}

void onRenderScene2() {
	glEnable(GL_MULTISAMPLE_ARB);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(theta, 0.5f, -1.5f, 0.0f);
	//glBegin (GL_TRIANGLES);
	//	glColor3f (1.0f, 0.0f, 0.0f);   glVertex3f (0.0f, 1.0f, 1.0f);
	//	glColor3f (0.0f, 1.0f, 0.0f);   glVertex3f (0.87f, -0.5f, -0.5f);
	//	glColor3f (0.0f, 0.0f, 1.0f);   glVertex3f (-0.87f, -0.5f, -0.5f);
	//glEnd ();
	//drawSphere(100, 100, vec3(0.75f, 0.75f, 0.75f));
	//drawCube(vec3(1.0f, 0.15f, 1.0f), 0x1|0x2|0x4);
	//drawRandPoints(100000);
	drawGrid();
	drawAxis();
	//drawTorus2(20, 11, vec3(0.5f, 0.5f, 0.5f));
	glPopMatrix();
	glDisable(GL_DEPTH_TEST);
	drawText("abcdefghijklmnopqrstuvwxyz\n", vec2(-1.f, 1.f), vec2(1.f, 1.f), vec4(1.f, 1.f, 1.f, 1.f), vec2(0.f, 0.f));
	glEnable(GL_DEPTH_TEST);

	theta -= 0.1f;

}

void onResize(int x, int y, WPARAM wParam, LPARAM lParam) {
	activecamera->aspect = max(float(x), 1.0f) / max(float(y), 1.0f);
	activecamera->calcMatrix();
	//printf("aspect:%f\n", activecamera->aspect);
	ctx.sx = x;
	ctx.sy = y;

	glViewport(0, 0, x, y);
	onRenderScene();
	SwapBuffers(ctx.hdc);

	printf("WM_SIZE: %i:%i\n", x, y);
}

void onMove(int x, int y, WPARAM wParam, LPARAM lParam) {
	printf("WM_MOVE\n");
}

void onPaint(PAINTSTRUCT* rc, WPARAM wParam, LPARAM lParam) {
	printf("WM_PAINT\n");
	onRenderScene();
}

void onMouseWhell(int val, WPARAM wParam, LPARAM lParam) {
	printf("WM_MOUSEWHEEL\n");
}

void onMouseMove(int x, int y, WPARAM wParam, LPARAM lParam) {

}

void onComand(int cmd, WPARAM wParam, LPARAM lParam) {
	printf("WM_COMMAND\n");
}

void onKeyPress(int key, WPARAM wParam, LPARAM lParam, pKeyMap keymap, UINT Message) {
	if(Message == WM_KEYDOWN)
	switch (key)
	{
		case VK_SUBTRACT: {
			activecamera->fov -= 0.01f;
			activecamera->calcMatrix();
			break;
		}
		case VK_ADD: {
			activecamera->fov += 0.01f;
			activecamera->calcMatrix();
			break;
		}
		case VK_UP: {
			activecamera->grot.x = clamp(activecamera->grot.x + 0.05f, -F_PI_2, F_PI_2);
			activecamera->calcMatrix();
			break;
		}
		case VK_DOWN: {
			activecamera->grot.x = clamp(activecamera->grot.x - 0.05f, -F_PI_2, F_PI_2);
			activecamera->calcMatrix();
			break;
		}
		case VK_LEFT: {
			activecamera->grot.y = fmodf(activecamera->grot.y + 0.05f, F_2PI);
			activecamera->calcMatrix();
			break;
		}
		case VK_RIGHT: {
			activecamera->grot.y = fmodf(activecamera->grot.y - 0.05f, F_2PI);
			activecamera->calcMatrix();
			break;
		}
		case 0x51: { // Q
			 torus->grot.y = fmodf( torus->grot.y - 0.05f, F_2PI);
			esfera->grot.y = fmodf(esfera->grot.y - 0.05f, F_2PI);
			 torus->grot.w = fmodf( torus->grot.w - 0.05f, F_2PI);
			esfera->grot.w = fmodf(esfera->grot.w - 0.05f, F_2PI);
			break;				 
		}						 
		case 0x45: { // E		 
			 torus->grot.y = fmodf( torus->grot.y + 0.05f, F_2PI);
			esfera->grot.y = fmodf(esfera->grot.y + 0.05f, F_2PI);
			 torus->grot.w = fmodf( torus->grot.w + 0.05f, F_2PI);
			esfera->grot.w = fmodf(esfera->grot.w + 0.05f, F_2PI);
			break;
		}
		case 0x52: { // R
			 torus->grot = vec4(0, 0, 0.00000001f, 0.0003333);
			esfera->grot = vec4(0, 0, 0.00000001f, 0.000333);
			break;
		}
		case 0x57: { // W
			 torus->grot.x = fmodf( torus->grot.x + 0.05f, F_2PI);
			esfera->grot.x = fmodf(esfera->grot.x + 0.05f, F_2PI);
			 torus->grot.w = fmodf( torus->grot.w + 0.05f, F_2PI);
			esfera->grot.w = fmodf(esfera->grot.w + 0.05f, F_2PI);
			break;				 
		}						 
		case 0x53: { // S		 
			 torus->grot.x = fmodf( torus->grot.x - 0.05f, F_2PI);
			esfera->grot.x = fmodf(esfera->grot.x - 0.05f, F_2PI);
			 torus->grot.w = fmodf( torus->grot.w - 0.05f, F_2PI);
			esfera->grot.w = fmodf(esfera->grot.w - 0.05f, F_2PI);
			break;				  
		}						  
		case 0x41: { // A		  
			 torus->grot.z = fmodf( torus->grot.z + 0.05f, F_2PI);
			esfera->grot.z = fmodf(esfera->grot.z + 0.05f, F_2PI);
			 torus->grot.w = fmodf( torus->grot.w + 0.05f, F_2PI);
			esfera->grot.w = fmodf(esfera->grot.w + 0.05f, F_2PI);
			break;				  
		}						  
		case 0x44: { // D		  
			 torus->grot.z = fmodf( torus->grot.z - 0.05f, F_2PI);
			esfera->grot.z = fmodf(esfera->grot.z - 0.05f, F_2PI);
			 torus->grot.w = fmodf( torus->grot.w - 0.05f, F_2PI);
			esfera->grot.w = fmodf(esfera->grot.w - 0.05f, F_2PI);
			break;
		}
		case VK_SPACE: {
			 torus->malhas[0]->mMaterial->detach();
			esfera->malhas[0]->mMaterial->detach();
			 torus->malhas[0]->mMaterial=new material("default");
			esfera->malhas[0]->mMaterial= new material("default");
			printf("Material Reloaded\n\n");
			// torus->malhas[0]->mMaterial->mShader->readSrcFromFilenames();
			// torus->malhas[0]->mMaterial->mShader->generateShadersAndProgram();
			//esfera->malhas[0]->mMaterial->mShader->readSrcFromFilenames();
			//esfera->malhas[0]->mMaterial->mShader->generateShadersAndProgram();
			break;
		}
		default: {
			break;
		}
	}
}

void decodeKeyPress(int key, WPARAM wParam, LPARAM lParam, pKeyMap keymap, UINT Message) {
	//char a[1000];
//printf("%i %i %i %i; %i %i %i %i %i; %i %i %i %i %i %i %i; \n", LOWORD (lParam), HIWORD (lParam),LOWORD (wParam), HIWORD (wParam), HIWORD (lParam)&0x01000000, HIWORD (lParam)&0x02000000, HIWORD (lParam)&0x04000000, HIWORD (lParam)&0x08000000, HIWORD (lParam)&0x10000000, GetKeyState (VK_SHIFT), GetKeyState (VK_LSHIFT), GetKeyState (VK_RSHIFT), GetKeyState (VK_LCONTROL), GetKeyState (VK_RCONTROL), GetKeyState (VK_LMENU), GetKeyState (VK_RMENU));
	//HIWORD (lParam)&0x02000000, HIWORD (lParam)&0x04000000, HIWORD (lParam)&0x08000000, HIWORD (lParam)&0x10000000);
//SetWindowTextA(hwnd, a);
	//keymap->up = (lParam & 0x80000000) >> 31;
	//keymap->down = !(lParam & 0x80000000) >> 31;
	//keymap->lalt = GetAsyncKeyState(VK_LMENU);
	//keymap->ralt = GetAsyncKeyState(VK_RMENU);
	//keymap->lctrl = GetAsyncKeyState(VK_LCONTROL);
	//keymap->lctrl = GetAsyncKeyState(VK_RCONTROL);
	//keymap->lshift = GetAsyncKeyState(VK_LSHIFT);
	//keymap->rshift = GetAsyncKeyState(VK_RSHIFT);
	////keymap->capsl = GetAsyncKeyState(VK_LMENU);
	////keymap->lalt = GetAsyncKeyState(VK_LMENU);

	if (wParam >= 0x30 && wParam <= 0x39)	// ASCII 0 - 9
		printf("* %c ", wParam);
	else if (wParam >= 0x41 && wParam <= 0x5A)	// ASCII A - Z
		printf("* %c ", wParam);
	else if (wParam >= 0x60 && wParam <= 0x69)	// NUMPAD 0 - 0
		printf("* NUMPAD %c ", wParam - 0x30);
	else if (wParam >= 0x70 && wParam <= 0x87)	// NUMPAD 0 - 0
		printf("* F%i ", wParam-0x6F);
	else
		switch (wParam)
		{
		case  VK_LBUTTON                           :	printf("VK_LBUTTON                      ");	break;
		case  VK_RBUTTON                           :	printf("VK_RBUTTON                      ");	break;
		case  VK_CANCEL                            :	printf("VK_CANCEL                       ");	break;
		case  VK_MBUTTON                           :	printf("VK_MBUTTON                      ");	break;
		case  VK_XBUTTON1                          :	printf("VK_XBUTTON1                     ");	break;
		case  VK_XBUTTON2                          :	printf("VK_XBUTTON2                     ");	break;
		case  VK_BACK                              :	printf("VK_BACK                         ");	break;
		case  VK_TAB                               :	printf("VK_TAB                          ");	break;
		case  VK_CLEAR                             :	printf("VK_CLEAR                        ");	break;
		case  VK_RETURN                            :	printf("VK_RETURN                       ");	break;
		case  VK_SHIFT                             :	printf("VK_SHIFT                        ");	break;
		case  VK_CONTROL                           :	printf("VK_CONTROL                      ");	break;
		case  VK_MENU                              :	printf("VK_MENU                         ");	break;
		case  VK_PAUSE                             :	printf("VK_PAUSE                        ");	break;
		case  VK_CAPITAL                           :	printf("VK_CAPITAL                      ");	break;
		case  VK_KANA                              :	printf("VK_KANA,VK_HANGEUL,VK_HANGUL    ");	break;
		case  VK_JUNJA                             :	printf("VK_JUNJA                        ");	break;
		case  VK_FINAL                             :	printf("VK_FINAL                        ");	break;
		case  VK_HANJA                             :	printf("VK_HANJA,VK_KANJI               ");	break;
		case  VK_ESCAPE                            :	printf("VK_ESCAPE                       ");	break;
		case  VK_CONVERT                           :	printf("VK_CONVERT                      ");	break;
		case  VK_NONCONVERT                        :	printf("VK_NONCONVERT                   ");	break;
		case  VK_ACCEPT                            :	printf("VK_ACCEPT                       ");	break;
		case  VK_MODECHANGE                        :	printf("VK_MODECHANGE                   ");	break;
		case  VK_SPACE                             :	printf("VK_SPACE                        ");	break;
		case  VK_PRIOR                             :	printf("VK_PRIOR                        ");	break;
		case  VK_NEXT                              :	printf("VK_NEXT                         ");	break;
		case  VK_END                               :	printf("VK_END                          ");	break;
		case  VK_HOME                              :	printf("VK_HOME                         ");	break;
		case  VK_LEFT                              :	printf("VK_LEFT                         ");	break;
		case  VK_UP                                :	printf("VK_UP                           ");	break;
		case  VK_RIGHT                             :	printf("VK_RIGHT                        ");	break;
		case  VK_DOWN                              :	printf("VK_DOWN                         ");	break;
		case  VK_SELECT                            :	printf("VK_SELECT                       ");	break;
		case  VK_PRINT                             :	printf("VK_PRINT                        ");	break;
		case  VK_EXECUTE                           :	printf("VK_EXECUTE                      ");	break;
		case  VK_SNAPSHOT                          :	printf("VK_SNAPSHOT                     ");	break;
		case  VK_INSERT                            :	printf("VK_INSERT                       ");	break;
		case  VK_DELETE                            :	printf("VK_DELETE                       ");	break;
		case  VK_HELP                              :	printf("VK_HELP                         ");	break;
		case  VK_LWIN                              :	printf("VK_LWIN                         ");	break;
		case  VK_RWIN                              :	printf("VK_RWIN                         ");	break;
		case  VK_APPS                              :	printf("VK_APPS                         ");	break;
		case  VK_SLEEP                             :	printf("VK_SLEEP                        ");	break;
		case  VK_MULTIPLY                          :	printf("VK_MULTIPLY                     ");	break;
		case  VK_ADD                               :	printf("VK_ADD                          ");	break;
		case  VK_SEPARATOR                         :	printf("VK_SEPARATOR                    ");	break;
		case  VK_SUBTRACT                          :	printf("VK_SUBTRACT                     ");	break;
		case  VK_DECIMAL                           :	printf("VK_DECIMAL                      ");	break;
		case  VK_DIVIDE                            :	printf("VK_DIVIDE                       ");	break;
		case  VK_NAVIGATION_VIEW                   :	printf("VK_NAVIGATION_VIEW              ");	break;
		case  VK_NAVIGATION_MENU                   :	printf("VK_NAVIGATION_MENU              ");	break;
		case  VK_NAVIGATION_UP                     :	printf("VK_NAVIGATION_UP                ");	break;
		case  VK_NAVIGATION_DOWN                   :	printf("VK_NAVIGATION_DOWN              ");	break;
		case  VK_NAVIGATION_LEFT                   :	printf("VK_NAVIGATION_LEFT              ");	break;
		case  VK_NAVIGATION_RIGHT                  :	printf("VK_NAVIGATION_RIGHT             ");	break;
		case  VK_NAVIGATION_ACCEPT                 :	printf("VK_NAVIGATION_ACCEPT            ");	break;
		case  VK_NAVIGATION_CANCEL                 :	printf("VK_NAVIGATION_CANCEL            ");	break;
		case  VK_NUMLOCK                           :	printf("VK_NUMLOCK                      ");	break;
		case  VK_SCROLL                            :	printf("VK_SCROLL                       ");	break;
		case  VK_OEM_NEC_EQUAL                     :	printf("VK_OEM_NEC_EQUAL,VK_OEM_FJ_JISHO");	break;
		case  VK_OEM_FJ_MASSHOU                    :	printf("VK_OEM_FJ_MASSHOU               ");	break;
		case  VK_OEM_FJ_TOUROKU                    :	printf("VK_OEM_FJ_TOUROKU               ");	break;
		case  VK_OEM_FJ_LOYA                       :	printf("VK_OEM_FJ_LOYA                  ");	break;
		case  VK_OEM_FJ_ROYA                       :	printf("VK_OEM_FJ_ROYA                  ");	break;
		case  VK_LSHIFT                            :	printf("VK_LSHIFT                       ");	break;
		case  VK_RSHIFT                            :	printf("VK_RSHIFT                       ");	break;
		case  VK_LCONTROL                          :	printf("VK_LCONTROL                     ");	break;
		case  VK_RCONTROL                          :	printf("VK_RCONTROL                     ");	break;
		case  VK_LMENU                             :	printf("VK_LMENU                        ");	break;
		case  VK_RMENU                             :	printf("VK_RMENU                        ");	break;
		case  VK_BROWSER_BACK                      :	printf("VK_BROWSER_BACK                 ");	break;
		case  VK_BROWSER_FORWARD                   :	printf("VK_BROWSER_FORWARD              ");	break;
		case  VK_BROWSER_REFRESH                   :	printf("VK_BROWSER_REFRESH              ");	break;
		case  VK_BROWSER_STOP                      :	printf("VK_BROWSER_STOP                 ");	break;
		case  VK_BROWSER_SEARCH                    :	printf("VK_BROWSER_SEARCH               ");	break;
		case  VK_BROWSER_FAVORITES                 :	printf("VK_BROWSER_FAVORITES            ");	break;
		case  VK_BROWSER_HOME                      :	printf("VK_BROWSER_HOME                 ");	break;
		case  VK_VOLUME_MUTE                       :	printf("VK_VOLUME_MUTE                  ");	break;
		case  VK_VOLUME_DOWN                       :	printf("VK_VOLUME_DOWN                  ");	break;
		case  VK_VOLUME_UP                         :	printf("VK_VOLUME_UP                    ");	break;
		case  VK_MEDIA_NEXT_TRACK                  :	printf("VK_MEDIA_NEXT_TRACK             ");	break;
		case  VK_MEDIA_PREV_TRACK                  :	printf("VK_MEDIA_PREV_TRACK             ");	break;
		case  VK_MEDIA_STOP                        :	printf("VK_MEDIA_STOP                   ");	break;
		case  VK_MEDIA_PLAY_PAUSE                  :	printf("VK_MEDIA_PLAY_PAUSE             ");	break;
		case  VK_LAUNCH_MAIL                       :	printf("VK_LAUNCH_MAIL                  ");	break;
		case  VK_LAUNCH_MEDIA_SELECT               :	printf("VK_LAUNCH_MEDIA_SELECT          ");	break;
		case  VK_LAUNCH_APP1                       :	printf("VK_LAUNCH_APP1                  ");	break;
		case  VK_LAUNCH_APP2                       :	printf("VK_LAUNCH_APP2                  ");	break;
		case  VK_OEM_1                             :	printf("VK_OEM_1                        ");	break;
		case  VK_OEM_PLUS                          :	printf("VK_OEM_PLUS                     ");	break;
		case  VK_OEM_COMMA                         :	printf("VK_OEM_COMMA                    ");	break;
		case  VK_OEM_MINUS                         :	printf("VK_OEM_MINUS                    ");	break;
		case  VK_OEM_PERIOD                        :	printf("VK_OEM_PERIOD                   ");	break;
		case  VK_OEM_2                             :	printf("VK_OEM_2                        ");	break;
		case  VK_OEM_3                             :	printf("VK_OEM_3                        ");	break;
		case  0xC1                                 :	printf("VK_/ VK_DIVIDE                  ");	break;
		case  VK_GAMEPAD_A                         :	printf("VK_GAMEPAD_A                    ");	break;
		case  VK_GAMEPAD_B                         :	printf("VK_GAMEPAD_B                    ");	break;
		case  VK_GAMEPAD_X                         :	printf("VK_GAMEPAD_X                    ");	break;
		case  VK_GAMEPAD_Y                         :	printf("VK_GAMEPAD_Y                    ");	break;
		case  VK_GAMEPAD_RIGHT_SHOULDER            :	printf("VK_GAMEPAD_RIGHT_SHOULDER       ");	break;
		case  VK_GAMEPAD_LEFT_SHOULDER             :	printf("VK_GAMEPAD_LEFT_SHOULDER        ");	break;
		case  VK_GAMEPAD_LEFT_TRIGGER              :	printf("VK_GAMEPAD_LEFT_TRIGGER         ");	break;
		case  VK_GAMEPAD_RIGHT_TRIGGER             :	printf("VK_GAMEPAD_RIGHT_TRIGGER        ");	break;
		case  VK_GAMEPAD_DPAD_UP                   :	printf("VK_GAMEPAD_DPAD_UP              ");	break;
		case  VK_GAMEPAD_DPAD_DOWN                 :	printf("VK_GAMEPAD_DPAD_DOWN            ");	break;
		case  VK_GAMEPAD_DPAD_LEFT                 :	printf("VK_GAMEPAD_DPAD_LEFT            ");	break;
		case  VK_GAMEPAD_DPAD_RIGHT                :	printf("VK_GAMEPAD_DPAD_RIGHT           ");	break;
		case  VK_GAMEPAD_MENU                      :	printf("VK_GAMEPAD_MENU                 ");	break;
		case  VK_GAMEPAD_VIEW                      :	printf("VK_GAMEPAD_VIEW                 ");	break;
		case  VK_GAMEPAD_LEFT_THUMBSTICK_BUTTON    :	printf("VK_GAMEPAD_LEFT_THUMBSTICK_BUTTO");	break;
		case  VK_GAMEPAD_RIGHT_THUMBSTICK_BUTTON   :	printf("VK_GAMEPAD_RIGHT_THUMBSTICK_BUTT");	break;
		case  VK_GAMEPAD_LEFT_THUMBSTICK_UP        :	printf("VK_GAMEPAD_LEFT_THUMBSTICK_UP   ");	break;
		case  VK_GAMEPAD_LEFT_THUMBSTICK_DOWN      :	printf("VK_GAMEPAD_LEFT_THUMBSTICK_DOWN ");	break;
		case  VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT     :	printf("VK_GAMEPAD_LEFT_THUMBSTICK_RIGHT");	break;
		case  VK_GAMEPAD_LEFT_THUMBSTICK_LEFT      :	printf("VK_GAMEPAD_LEFT_THUMBSTICK_LEFT ");	break;
		case  VK_GAMEPAD_RIGHT_THUMBSTICK_UP       :	printf("VK_GAMEPAD_RIGHT_THUMBSTICK_UP  ");	break;
		case  VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN     :	printf("VK_GAMEPAD_RIGHT_THUMBSTICK_DOWN");	break;
		case  VK_GAMEPAD_RIGHT_THUMBSTICK_RIGHT    :	printf("VK_GAMEPAD_RIGHT_THUMBSTICK_RIGH");	break;
		case  VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT     :	printf("VK_GAMEPAD_RIGHT_THUMBSTICK_LEFT");	break;
		case  VK_OEM_4                             :	printf("VK_OEM_4                        ");	break;
		case  VK_OEM_5                             :	printf("VK_OEM_5                        ");	break;
		case  VK_OEM_6                             :	printf("VK_OEM_6                        ");	break;
		case  VK_OEM_7                             :	printf("VK_OEM_7                        ");	break;
		case  VK_OEM_8                             :	printf("VK_OEM_8                        ");	break;
		case  VK_OEM_AX                            :	printf("VK_OEM_AX                       ");	break;
		case  VK_OEM_102                           :	printf("VK_OEM_102                      ");	break;
		case  VK_ICO_HELP                          :	printf("VK_ICO_HELP                     ");	break;
		case  VK_ICO_00                            :	printf("VK_ICO_00                       ");	break;
		case  VK_PROCESSKEY                        :	printf("VK_PROCESSKEY                   ");	break;
		case  VK_ICO_CLEAR                         :	printf("VK_ICO_CLEAR                    ");	break;
		case  VK_PACKET                            :	printf("VK_PACKET                       ");	break;
		case  VK_OEM_RESET                         :	printf("VK_OEM_RESET                    ");	break;
		case  VK_OEM_JUMP                          :	printf("VK_OEM_JUMP                     ");	break;
		case  VK_OEM_PA1                           :	printf("VK_OEM_PA1                      ");	break;
		case  VK_OEM_PA2                           :	printf("VK_OEM_PA2                      ");	break;
		case  VK_OEM_PA3                           :	printf("VK_OEM_PA3                      ");	break;
		case  VK_OEM_WSCTRL                        :	printf("VK_OEM_WSCTRL                   ");	break;
		case  VK_OEM_CUSEL                         :	printf("VK_OEM_CUSEL                    ");	break;
		case  VK_OEM_ATTN                          :	printf("VK_OEM_ATTN                     ");	break;
		case  VK_OEM_FINISH                        :	printf("VK_OEM_FINISH                   ");	break;
		case  VK_OEM_COPY                          :	printf("VK_OEM_COPY                     ");	break;
		case  VK_OEM_AUTO                          :	printf("VK_OEM_AUTO                     ");	break;
		case  VK_OEM_ENLW                          :	printf("VK_OEM_ENLW                     ");	break;
		case  VK_OEM_BACKTAB                       :	printf("VK_OEM_BACKTAB                  ");	break;
		case  VK_ATTN                              :	printf("VK_ATTN                         ");	break;
		case  VK_CRSEL                             :	printf("VK_CRSEL                        ");	break;
		case  VK_EXSEL                             :	printf("VK_EXSEL                        ");	break;
		case  VK_EREOF                             :	printf("VK_EREOF                        ");	break;
		case  VK_PLAY                              :	printf("VK_PLAY                         ");	break;
		case  VK_ZOOM                              :	printf("VK_ZOOM                         ");	break;
		case  VK_NONAME                            :	printf("VK_NONAME                       ");	break;
		case  VK_PA1                               :	printf("VK_PA1                          ");	break;
		case  VK_OEM_CLEAR                         :	printf("VK_OEM_CLEAR                    ");	break;
						                     
		default:
			printf("???????????"); break;
		}

		printf("UP:%x pDown:%x alt:%x rep:%i scOEM:%x extended:%x 0x%x 0x%x '", 
			(lParam >> 31) & 0x1, 
			(lParam >> 30) & 0x1, 
			(lParam >> 29) & 0x1, 
			lParam & 0xffff, 
			(lParam >> 16) & 0xff, 
			(lParam >> 24) & 0x1, 
			lParam, 
			wParam);
		//printf("%c", lParam);
		//printf(" '");
		printf("%c", wParam);
		printf("' ");
		printf("L: %i%i%i%i %i%i%i%i:%i%i%i%i %i%i%i%i  %i%i%i%i %i%i%i%i:%i%i%i%i %i%i%i%i"
			//"    W: %i%i%i%i %i%i%i%i:%i%i%i%i %i%i%i%i  %i%i%i%i %i%i%i%i:%i%i%i%i %i%i%i%i"
			"\n\n",
			(lParam >> 31) & 0x1, (lParam >> 30) & 0x1, (lParam >> 29) & 0x1, (lParam >> 28) & 0x1,
			(lParam >> 27) & 0x1, (lParam >> 26) & 0x1, (lParam >> 25) & 0x1, (lParam >> 24) & 0x1,

			(lParam >> 23) & 0x1, (lParam >> 22) & 0x1, (lParam >> 21) & 0x1, (lParam >> 20) & 0x1,
			(lParam >> 19) & 0x1, (lParam >> 18) & 0x1, (lParam >> 17) & 0x1, (lParam >> 16) & 0x1,

			(lParam >> 15) & 0x1, (lParam >> 14) & 0x1, (lParam >> 13) & 0x1, (lParam >> 12) & 0x1,
			(lParam >> 11) & 0x1, (lParam >> 10) & 0x1, (lParam >> 9) & 0x1, (lParam >> 8) & 0x1,

			(lParam >> 7) & 0x1, (lParam >> 6) & 0x1, (lParam >> 5) & 0x1, (lParam >> 4) & 0x1,
			(lParam >> 3) & 0x1, (lParam >> 2) & 0x1, (lParam >> 1) & 0x1, (lParam >> 0) & 0x1);

			//(lParam & 0x01000000) >> 24, (lParam & 0x02000000) >> 25, (lParam & 0x04000000) >> 26, (lParam & 0x08000000) >> 27,
			//(lParam & 0x10000000) >> 28, (lParam & 0x20000000) >> 29, (lParam & 0x00000000) >> 30, (lParam & 0x80000000) >> 31, // 0x20000000 = alt


			 //(wParam & 0x80000000) >> 31, (wParam & 0x40000000) >> 30, (wParam & 0x20000000) >> 29, (wParam & 0x10000000) >> 28, 
			 //(wParam & 0x08000000) >> 27, (wParam & 0x04000000) >> 26, (wParam & 0x02000000) >> 25, (wParam & 0x01000000) >> 24, 

			 //(wParam & 0x00800000) >> 23, (wParam & 0x00400000) >> 22, (wParam & 0x00200000) >> 21, (wParam & 0x00100000) >> 20, 
			 //(wParam & 0x00080000) >> 19, (wParam & 0x00040000) >> 18, (wParam & 0x00020000) >> 17, (wParam & 0x00010000) >> 16, 

			 //(wParam & 0x00008000) >> 15, (wParam & 0x00004000) >> 14, (wParam & 0x00002000) >> 13, (wParam & 0x00001000) >> 12, 
			 //(wParam & 0x00000800) >> 11, (wParam & 0x00000400) >> 10, (wParam & 0x00000200) >> 9,  (wParam & 0x00000100) >> 8,  

			 //(wParam & 0x00000080) >> 7,  (wParam & 0x00000040) >> 6,  (wParam & 0x00000020) >> 5,  (wParam & 0x00000010) >> 4,   // 0x00000040 = ctrl
			 //(wParam & 0x00000008) >> 3,  (wParam & 0x00000004) >> 2,  (wParam & 0x00000002) >> 1,  (wParam & 0x00000001) >> 0 ); // 0x00000020 = shift



			//(lParam >> 0) & 0x1, (lParam >> 1) & 0x1, (lParam >> 2) & 0x1, (lParam >> 3) & 0x1,
			//(lParam >> 4) & 0x1, (lParam >> 5) & 0x1, (lParam >> 6) & 0x1, (lParam >> 7) & 0x1,
			//(lParam >> 8) & 0x1, (lParam >> 9) & 0x1, (lParam >> 10) & 0x1, (lParam >> 11) & 0x1,
			//(lParam >> 12) & 0x1, (lParam >> 13) & 0x1, (lParam >> 14) & 0x1, (lParam >> 15) & 0x1,
			//(lParam >> 16) & 0x1, (lParam >> 17) & 0x1, (lParam >> 18) & 0x1, (lParam >> 19) & 0x1,
			//(lParam >> 20) & 0x1, (lParam >> 21) & 0x1, (lParam >> 22) & 0x1, (lParam >> 23) & 0x1,
			//(lParam >> 24) & 0x1, (lParam >> 25) & 0x1, (lParam >> 26) & 0x1, (lParam >> 27) & 0x1,
			//(lParam >> 28) & 0x1, (lParam >> 29) & 0x1, (lParam >> 30) & 0x1, (lParam >> 31) & 0x1);

			//(lParam & 0x01000000) >> 24, (lParam & 0x02000000) >> 25, (lParam & 0x04000000) >> 26, (lParam & 0x08000000) >> 27,
			//(lParam & 0x10000000) >> 28, (lParam & 0x20000000) >> 29, (lParam & 0x00000000) >> 30, (lParam & 0x80000000) >> 31, // 0x20000000 = alt

			//(wParam & 0x00000001) >> 0, (wParam & 0x00000002) >> 1, (wParam & 0x00000004) >> 2, (wParam & 0x00000008) >> 3, // 0x00000020 = shift
			//(wParam & 0x00000010) >> 4, (wParam & 0x00000020) >> 5, (wParam & 0x00000040) >> 6, (wParam & 0x00000080) >> 7, // 0x00000040 = ctrl
			//(wParam & 0x00000100) >> 8, (wParam & 0x00000200) >> 9, (wParam & 0x00000400) >> 10, (wParam & 0x00000800) >> 11,
			//(wParam & 0x00001000) >> 12, (wParam & 0x00002000) >> 13, (wParam & 0x00004000) >> 14, (wParam & 0x00008000) >> 15,
			//(wParam & 0x00010000) >> 16, (wParam & 0x00020000) >> 17, (wParam & 0x00040000) >> 18, (wParam & 0x00080000) >> 19,
			//(wParam & 0x00100000) >> 20, (wParam & 0x00200000) >> 21, (wParam & 0x00400000) >> 22, (wParam & 0x00800000) >> 23,
			//(wParam & 0x01000000) >> 24, (wParam & 0x02000000) >> 25, (wParam & 0x04000000) >> 26, (wParam & 0x08000000) >> 27,
			//(wParam & 0x10000000) >> 28, (wParam & 0x20000000) >> 29, (wParam & 0x40000000) >> 30, (wParam & 0x80000000) >> 31);

	//onKeyPress(wParam, wParam, lParam, keymap, Message);
}

void decodeKeyPress2(pKeyMap keymap) {
	keymap->lshift     = GetAsyncKeyState(VK_LSHIFT);
	keymap->rshift     = GetAsyncKeyState(VK_RSHIFT);
	keymap->lctrl      = GetAsyncKeyState(VK_LCONTROL);
	keymap->rctrl      = GetAsyncKeyState(VK_RCONTROL);
	keymap->lalt       = GetAsyncKeyState(VK_LMENU);
	keymap->ralt       = GetAsyncKeyState(VK_RMENU);
	keymap->alt        = keymap->lalt || keymap->ralt || GetAsyncKeyState(VK_MENU);

	keymap->space      = GetAsyncKeyState(VK_SPACE);
	keymap->win        = GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN);
	keymap->capsl      = GetAsyncKeyState(VK_CAPITAL);
	keymap->numl       = GetAsyncKeyState(VK_NUMLOCK);
	keymap->scroll     = GetAsyncKeyState(VK_SCROLL);
	keymap->up         = 0;
	keymap->down       = 0;

	WPARAM wParam = 0;

	if (GetAsyncKeyState( VK_SUBTRACT))
	{
		activecamera->fov -= 0.01f;
		activecamera->calcMatrix();
	}
	if (GetAsyncKeyState(VK_ADD))
	{
		activecamera->fov += 0.01f;
		activecamera->calcMatrix();
	}
	if (GetAsyncKeyState(VK_UP))
	{
		activecamera->grot.x = clamp(activecamera->grot.x + 0.05f, -F_PI_2, F_PI_2);
		activecamera->calcMatrix();
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		activecamera->grot.x = clamp(activecamera->grot.x - 0.05f, -F_PI_2, F_PI_2);
		activecamera->calcMatrix();
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		activecamera->grot.y = fmodf(activecamera->grot.y + 0.05f, F_2PI);
		activecamera->calcMatrix();
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		activecamera->grot.y = fmodf(activecamera->grot.y - 0.05f, F_2PI);
		activecamera->calcMatrix();
	}
	if (GetAsyncKeyState(0x51))
	{ // Q
		torus->grot.y = fmodf(torus->grot.y - 0.05f, F_2PI);
		esfera->grot.y = fmodf(esfera->grot.y - 0.05f, F_2PI);
		torus->grot.w = fmodf(torus->grot.w - 0.05f, F_2PI);
		esfera->grot.w = fmodf(esfera->grot.w - 0.05f, F_2PI);
	}
	if (GetAsyncKeyState(0x45))
	{ // E		 
		torus->grot.y = fmodf(torus->grot.y + 0.05f, F_2PI);
		esfera->grot.y = fmodf(esfera->grot.y + 0.05f, F_2PI);
		torus->grot.w = fmodf(torus->grot.w + 0.05f, F_2PI);
		esfera->grot.w = fmodf(esfera->grot.w + 0.05f, F_2PI);
	}
	if (GetAsyncKeyState(0x52))
	{ // R
		torus->grot = vec4(0, 0, 0.00000001f, 0.0003333);
		esfera->grot = vec4(0, 0, 0.00000001f, 0.000333);
	}
	if (GetAsyncKeyState(0x57))
	{ // W
		torus->grot.x = fmodf(torus->grot.x + 0.05f, F_2PI);
		esfera->grot.x = fmodf(esfera->grot.x + 0.05f, F_2PI);
		torus->grot.w = fmodf(torus->grot.w + 0.05f, F_2PI);
		esfera->grot.w = fmodf(esfera->grot.w + 0.05f, F_2PI);
	}
	if (GetAsyncKeyState(0x53))
	{ // S		 
		torus->grot.x = fmodf(torus->grot.x - 0.05f, F_2PI);
		esfera->grot.x = fmodf(esfera->grot.x - 0.05f, F_2PI);
		torus->grot.w = fmodf(torus->grot.w - 0.05f, F_2PI);
		esfera->grot.w = fmodf(esfera->grot.w - 0.05f, F_2PI);
	}
	if (GetAsyncKeyState(0x41))
	{ // A		  
		torus->grot.z = fmodf(torus->grot.z + 0.05f, F_2PI);
		esfera->grot.z = fmodf(esfera->grot.z + 0.05f, F_2PI);
		torus->grot.w = fmodf(torus->grot.w + 0.05f, F_2PI);
		esfera->grot.w = fmodf(esfera->grot.w + 0.05f, F_2PI);
	}
	if (GetAsyncKeyState(0x44))
	{ // D		  
		torus->grot.z = fmodf(torus->grot.z - 0.05f, F_2PI);
		esfera->grot.z = fmodf(esfera->grot.z - 0.05f, F_2PI);
		torus->grot.w = fmodf(torus->grot.w - 0.05f, F_2PI);
		esfera->grot.w = fmodf(esfera->grot.w - 0.05f, F_2PI);
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		torus->malhas[0]->mMaterial->detach();
		esfera->malhas[0]->mMaterial->detach();
		torus->malhas[0]->mMaterial = new material("default");
		esfera->malhas[0]->mMaterial = new material("default");
		printf("Material Reloaded\n\n");
	}
}

void onMousePress(int x, int y, WPARAM wParam, LPARAM lParam, int up, int n) {

}





























/*



#define MAX_LOADSTRING 100
//typedef struct GLContextStruct
//{
//    HWND hwnd;
//    HDC hdc;
//    HGLRC hglrc;
//    GLuint sx;
//    GLuint sy;
//    WNDCLASS wc;
//
//} GLContext;

// Vari??veis Globais:
HINSTANCE hInst;                                // inst??ncia atual
WCHAR szTitle[MAX_LOADSTRING];                  // O texto da barra de t??tulo
WCHAR szWindowClass[MAX_LOADSTRING];            // o nome da classe da janela principal

// Declara????es de encaminhamento de fun????es inclu??das nesse m??dulo de c??digo:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc2(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//GLContext ctx;
//int bQuit = false;
int startOpengl2() {
	//mat4x4f h;
	//h.row0=0.0f;
	//h.aray[7]=8;
	//h.mt[1][1]=5;
	//h.x[1]=5;
	//h.
	vec4 g[10];
	//g[1] = vec4(1.0f,2.0f,3.0f,4.0f);
	//{
	//	text i;


	//}

	ctx.sx = 512;
	ctx.sy = 512;
	ZeroMemory(&ctx.wc, sizeof(WNDCLASS));
	ctx.wc.hInstance = GetModuleHandle(NULL);
	ctx.wc.lpfnWndProc = WndProc;
	ctx.wc.lpszClassName = L"TestOpengl_01";
	ctx.wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	ctx.wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wc.style = CS_HREDRAW | CS_VREDRAW;
	ctx.wc.hIcon = LoadIcon(NULL, IDI_ASTERISK);
	if (0 == RegisterClass(&ctx.wc)) return GL_TRUE;
	// create window
	ctx.hwnd = CreateWindow(L"TestOpengl_01", L"Test Opengl 01",
		WS_OVERLAPPEDWINDOW,
		111, 202, ctx.sx, ctx.sy,
		NULL, NULL, NULL, NULL);

	// get the device context
	ctx.hdc = GetDC(ctx.hwnd);
	// WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL

	// find pixel format

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;
	int visual = ChoosePixelFormat(ctx.hdc, &pfd);
	// set the pixel format for the dc 
	//SetPixelFormat(ctx.hdc, visual, &pfd);
	SetPixelFormat(ctx.hdc, 80, &pfd); // it is a hack, so i know that this number 80 activate the 4x AntiAlias on my computer
	// create rendering context 
	ctx.hglrc = wglCreateContext(ctx.hdc);
	wglMakeCurrent(ctx.hdc, ctx.hglrc);
	LoadWGL();
	LoadOGL();

	printf(wglGetExtensionsStringARB(ctx.hdc));

	int attributes[] = {
	  WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	  WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	  WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
	  WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
	  WGL_COLOR_BITS_ARB, 32,
	  WGL_DEPTH_BITS_ARB, 24,
	  WGL_STENCIL_BITS_ARB, 8,
	  WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
	  WGL_SAMPLES_ARB, 4,        // Number of samples
	  0
	};

	int attrib[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
	int nResults[1] = { 0 };
	int pixFmt = 1;
	wglGetPixelFormatAttribivARB(ctx.hdc, pixFmt, 0, 1, attrib, nResults);


	GLint pfAttribCount[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
	//GLint pfAttribList[] = { 
	//	WGL_DRAW_TO_WINDOW_ARB, 
	//	WGL_ACCELERATION_ARB,
	//	WGL_SUPPORT_OPENGL_ARB, 
	//	WGL_DOUBLE_BUFFER_ARB, 
	//	WGL_DEPTH_BITS_ARB, 
	//	WGL_STENCIL_BITS_ARB, 
	//	WGL_RED_BITS_ARB, 
	//	WGL_GREEN_BITS_ARB, 
	//	WGL_BLUE_BITS_ARB, 
	//	WGL_ALPHA_BITS_ARB 
	// };
	GLint pfAttribList[] = {
		//WGL_NUMBER_PIXEL_FORMATS_ARB,
		//WGL_SUPPORT_OPENGL_ARB,
		//WGL_DOUBLE_BUFFER_ARB,
		//WGL_DRAW_TO_WINDOW_ARB,
		//WGL_DRAW_TO_BITMAP_ARB,
		//WGL_ACCELERATION_ARB,
		//WGL_NUMBER_OVERLAYS_ARB,
		//WGL_NUMBER_UNDERLAYS_ARB,
		//WGL_TRANSPARENT_ARB,
		//WGL_STEREO_ARB,
		//WGL_COLOR_BITS_ARB,
		//WGL_RED_BITS_ARB,
		//WGL_RED_SHIFT_ARB,
		//WGL_GREEN_BITS_ARB,
		//WGL_GREEN_SHIFT_ARB,
		//WGL_BLUE_BITS_ARB,
		//WGL_BLUE_SHIFT_ARB,
		//WGL_ALPHA_BITS_ARB,
		//WGL_ALPHA_SHIFT_ARB,
		//WGL_ACCUM_BITS_ARB,
		//WGL_ACCUM_RED_BITS_ARB,
		//WGL_ACCUM_GREEN_BITS_ARB,
		//WGL_ACCUM_BLUE_BITS_ARB,
		//WGL_ACCUM_ALPHA_BITS_ARB,
		//WGL_DEPTH_BITS_ARB,
		//WGL_STENCIL_BITS_ARB,
		//WGL_SHARE_DEPTH_ARB,
		//WGL_SHARE_STENCIL_ARB,
		//WGL_SHARE_ACCUM_ARB,
		//WGL_PIXEL_TYPE_ARB,
		//WGL_TRANSPARENT_RED_VALUE_ARB,
		//WGL_TRANSPARENT_GREEN_VALUE_ARB,
		//WGL_TRANSPARENT_BLUE_VALUE_ARB,
		//WGL_TRANSPARENT_ALPHA_VALUE_ARB,
		//WGL_TRANSPARENT_INDEX_VALUE_ARB,
		//WGL_NEED_PALETTE_ARB,
		//WGL_NEED_SYSTEM_PALETTE_ARB,
		//WGL_SWAP_LAYER_BUFFERS_ARB,
		//WGL_SWAP_METHOD_ARB,
		//WGL_SUPPORT_GDI_ARB,
		//WGL_AUX_BUFFERS_ARB,
		//WGL_SAMPLES_ARB

			WGL_SUPPORT_OPENGL_ARB,
			WGL_SUPPORT_GDI_ARB,
			WGL_DOUBLE_BUFFER_ARB,
			WGL_DRAW_TO_WINDOW_ARB,
			WGL_DRAW_TO_BITMAP_ARB,
			WGL_SWAP_METHOD_ARB,
			WGL_PIXEL_TYPE_ARB,
			WGL_AUX_BUFFERS_ARB,
			WGL_COLOR_BITS_ARB,
			WGL_DEPTH_BITS_ARB,
			WGL_STENCIL_BITS_ARB,
			WGL_RED_BITS_ARB,
			WGL_GREEN_BITS_ARB,
			WGL_BLUE_BITS_ARB,
			WGL_ALPHA_BITS_ARB,
			WGL_ACCUM_BITS_ARB,
			WGL_ACCUM_RED_BITS_ARB,
			WGL_ACCUM_GREEN_BITS_ARB,
			WGL_ACCUM_BLUE_BITS_ARB,
			WGL_ACCUM_ALPHA_BITS_ARB,
			WGL_SAMPLES_ARB,
	};

	FILE* LogPixelFormat = fopen("LogPixelFormat.py", "wb");



	int nPixelFormatCount = 0;
	wglGetPixelFormatAttribivARB(ctx.hdc, 1, 0, 1, pfAttribCount, &nPixelFormatCount);


	fclose(LogPixelFormat);
	return 1;
}




int APIENTRY mWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	startOpengl();
	GLuint hdrTextures[10];
	glEnable(GL_MULTISAMPLE);
	glGenTextures(1, hdrTextures);

	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hdrTextures[0]);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGB16F, ctx.sx, ctx.sy, GL_FALSE);

	GLint buf, sbuf;
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glGetIntegerv(GL_SAMPLE_BUFFERS, &buf);
	printf("number of sample buffers is %d\n", buf);
	glGetIntegerv(GL_SAMPLES, &sbuf);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, -2.0, 100.0);
	//glRotatef (1.0f, 0.1f, 0.0f, 0.0f);

	//int nvert=0, nindex=0;
	//vec3 *vert;
	//usvec3 *index;

	//for(int i=0; i<nvert; i++){
	//	printf("%i   %f %f %f\n", i, vert[i].x,vert[i].y,vert[i].z);
	//}

	//printf("test\n test");
	ShowWindow(ctx.hwnd, 1);



    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Coloque o c??digo aqui.

    // Inicializar cadeias de caracteres globais
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSOPENGL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realize a inicializa????o do aplicativo:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSOPENGL));

    MSG msg;

    // Loop de mensagem principal:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  FUN????O: MyRegisterClass()
//
//  FINALIDADE: Registra a classe de janela.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSOPENGL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSOPENGL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUN????O: InitInstance(HINSTANCE, int)
//
//   FINALIDADE: Salva o identificador de inst??ncia e cria a janela principal
//
//   COMENT??RIOS:
//
//        Nesta fun????o, o identificador de inst??ncias ?? salvo em uma vari??vel global e
//        crie e exiba a janela do programa principal.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Armazenar o identificador de inst??ncia em nossa vari??vel global

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUN????O: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  FINALIDADE: Processa as mensagens para a janela principal.
//
//  WM_COMMAND  - processar o menu do aplicativo
//  WM_PAINT    - Pintar a janela principal
//  WM_DESTROY  - postar uma mensagem de sa??da e retornar
//
//
LRESULT CALLBACK WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analise as sele????es do menu:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Adicione qualquer c??digo de desenho que use hdc aqui...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Manipulador de mensagem para a caixa 'sobre'.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
//*/




