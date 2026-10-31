/* HobTools - noclip + free camera for Hob (Runic Games, PC/Epic build 34091)
 * Loaded by Ogre as a plugin (listed in plugins.cfg). No CRT, plain Win32.
 *
 *   F6  toggle noclip   (WASD move relative to camera, Space/E up, Ctrl/Q down)
 *   F7  toggle free cam (WASD fly, Space/E up, Ctrl/Q down,
 *                        hold right mouse or arrow keys to look)
 *   Shift = 4x faster, PageUp / PageDown = change base speed
 */
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32;
typedef long long i64; typedef int BOOL; typedef void* HANDLE;
#define WINAPI __stdcall
#define TC __thiscall

/* ---- Win32 imports ---- */
__declspec(dllimport) HANDLE WINAPI GetModuleHandleA(const char*);
__declspec(dllimport) void*  WINAPI GetProcAddress(HANDLE, const char*);
__declspec(dllimport) BOOL   WINAPI VirtualProtect(void*, u32, u32, u32*);
__declspec(dllimport) BOOL   WINAPI QueryPerformanceCounter(i64*);
__declspec(dllimport) BOOL   WINAPI QueryPerformanceFrequency(i64*);
__declspec(dllimport) u32    WINAPI GetCurrentProcessId(void);
__declspec(dllimport) BOOL   WINAPI Beep(u32, u32);
__declspec(dllimport) HANDLE WINAPI CreateFileA(const char*, u32, u32, void*, u32, u32, HANDLE);
__declspec(dllimport) BOOL   WINAPI WriteFile(HANDLE, const void*, u32, u32*, void*);
__declspec(dllimport) u32    WINAPI SetFilePointer(HANDLE, long, long*, u32);
__declspec(dllimport) BOOL   WINAPI CloseHandle(HANDLE);
__declspec(dllimport) short  WINAPI GetAsyncKeyState(int);
__declspec(dllimport) HANDLE WINAPI GetForegroundWindow(void);
__declspec(dllimport) u32    WINAPI GetWindowThreadProcessId(HANDLE, u32*);
typedef struct { long x, y; } POINT; typedef struct { long l, t, r, b; } RECT;
__declspec(dllimport) BOOL WINAPI ScreenToClient(HANDLE, POINT*);
__declspec(dllimport) BOOL WINAPI GetClientRect(HANDLE, RECT*);
__declspec(dllimport) BOOL   WINAPI GetCursorPos(POINT*);

int _fltused = 0;
void* memset(void* d, int c, u32 n) { u8* p = d; while (n--) *p++ = (u8)c; return d; }
void* memcpy(void* d, const void* s, u32 n) { u8* a = d; const u8* b = s; while (n--) *a++ = *b++; return d; }
static int memcmp_(const void* x, const void* y, u32 n) { const u8* a = x; const u8* b = y; while (n--) { if (*a != *b) return *a - *b; a++, b++; } return 0; }

/* ---- tiny math (no libm) ---- */
#define PI 3.14159265f
static float fabsf_(float x) { return x < 0 ? -x : x; }
static float sqrtf_(float x) { float r; __asm__("sqrtss %1, %0" : "=x"(r) : "x"(x)); return r; }
static float wrap_pi(float a) { while (a > PI) a -= 2 * PI; while (a < -PI) a += 2 * PI; return a; }
static float sinf_(float x) { x = wrap_pi(x); float x2 = x * x;
    return x * (1 - x2 / 6 * (1 - x2 / 20 * (1 - x2 / 42 * (1 - x2 / 72 * (1 - x2 / 110))))); }
static float cosf_(float x) { return sinf_(x + PI / 2); }
static float atanf_(float x) { /* |x|<=1 approx, good to ~1e-4 */
    float x2 = x * x; return x * (0.99997726f + x2 * (-0.33262347f + x2 * (0.19354346f + x2 * (-0.11643287f + x2 * (0.05265332f - 0.01172120f * x2))))); }
static float atan2f_(float y, float x) {
    if (x == 0 && y == 0) return 0;
    if (fabsf_(x) >= fabsf_(y)) { float a = atanf_(y / x); if (x < 0) a += (y >= 0) ? PI : -PI; return a; }
    float a = PI / 2 - atanf_(x / y); return y < 0 ? a - PI : a; }
static float asinf_(float s) { if (s > 1) s = 1; if (s < -1) s = -1; return atan2f_(s, sqrtf_(1 - s * s)); }

/* ---- logging to HobTools.log in game folder ---- */
static HANDLE g_log = (HANDLE)-1;
static void logs(const char* s) {
    if (g_log == (HANDLE)-1) return; u32 n = 0, w; while (s[n]) n++; WriteFile(g_log, s, n, &w, 0);
}
static void logx(const char* s, u32 v) {
    char b[16]; int i; logs(s); b[0] = '0'; b[1] = 'x';
    for (i = 0; i < 8; i++) { u32 d = (v >> (28 - 4 * i)) & 15; b[2 + i] = (char)(d < 10 ? '0' + d : 'a' + d - 10); }
    b[10] = '\r'; b[11] = '\n'; b[12] = 0; logs(b);
}

/* ---- Ogre function pointers ---- */
typedef void (TC *CamVecFn)(void*, const float*);
typedef const float* (TC *CamGetFn)(void*);
typedef void* (TC *PtrGetFn)(void*);
typedef int (TC *IntGetFn)(void*);
typedef char (TC *RootFireFn)(void*);
static CamVecFn o_setPos, o_setOri, o_lookAt, o_yaw, o_pitch;
static CamGetFn o_getPos, o_getOri, o_getDerOri;
static PtrGetFn o_getViewport; static IntGetFn o_vpWidth;
static RootFireFn o_fireStarted;

/* ---- game (HOB.exe) functions, by RVA, verified by signature ---- */
static u8* g_base;
typedef void* (*GetPlayerFn)(void);
typedef const float* (TC *UnitGetPosFn)(void*, char);
typedef void (TC *UnitBoolFn)(void*, char);
typedef void (TC *UnitSetPosFn)(void*, const float*);
typedef void (*InputClearFn)(u32, u32);
static GetPlayerFn g_getPlayer; static UnitGetPosFn g_unitGetPos;
static UnitBoolFn g_flagBD; static InputClearFn g_inputClear;
static int g_gameOk;

static int sig(u32 rva, const char* hex, int n) {
    const u8* p = g_base + rva; int i;
    for (i = 0; i < n; i++) {
        int hi = hex[2 * i], lo = hex[2 * i + 1];
        hi = hi <= '9' ? hi - '0' : hi - 'a' + 10; lo = lo <= '9' ? lo - '0' : lo - 'a' + 10;
        if (p[i] != (u8)(hi * 16 + lo)) return 0;
    }
    return 1;
}

/* ---- supported game builds. All values are RVAs (VA - 0x400000). ---- */
typedef struct {
    const char* name;
    u32 getPlayer, unitGetPos, flagBD, inputClear;      /* player helpers */
    u32 conUpdate, setInput, conVtable;                 /* console text box */
    u32 pauseCmp;                                       /* pause-menu fix: cmp [eax+0xc0],0 / je */
    u32 logRefresh, logScroll, logAdd;                  /* console log widget */
    u32 stoi, cmdTable, cmdLo, cmdHi, textEnd;          /* console safety net */
    u32 setUpgrade, getAllUpgrades;                     /* GetAllUpgrades fix */
    u32 shopTable, prereqMet, getSave;                  /* shop item table, prerequisite check, save data */
    u32 langMgr;                                        /* returns the language manager (mov eax,[g]; ret) */
} Build;
static const Build g_builds[] = {
    { "Epic 1.17.3", 0x2b1fc0, 0x3ec050, 0x017660, 0x2cc250, 0x52b180, 0x52b3b0, 0x2e3b87c, 0x549725,
      0x58e870, 0x58e810, 0x58e790, 0x6b5d60, 0x39abb78, 0xbf000, 0xcb000, 0x25ce000, 0x2b7d00, 0x0c6ec0, 0x467afd8, 0x2b7350, 0x495f50, 0x4ae8d0 },
    { "Steam 1.17.2",    0x00a030, 0x3ec3e0, 0x017440, 0x2cb6f0, 0x52c3d0, 0x52c600, 0x2e32d2c, 0x54aa65,
      0x58ff10, 0x58feb0, 0x58fe30, 0x6b7310, 0x39a1a18, 0xbd800, 0xca000, 0x25c5000, 0x2b7180, 0x0c5960, 0x4670e28, 0x2b67d0, 0x496720, 0x4af1b0 },
};
static const Build* g_b;
static int build_matches(const Build* b) {
    const u8* gp = g_base + b->getPlayer;
    return gp[0] == 0xe8 && sig(b->getPlayer + 5, "85c0740c", 4) && sig(b->unitGetPos, "568bf18b4e7885c975", 9)
        && sig(b->flagBD, "8a4424048881bd000000c204", 12) && sig(b->inputClear, "8b4c24048b442408f7d1", 10)
        && sig(b->conUpdate, "56578bf1e8", 5) && sig(b->pauseCmp, "83b8c0000000007439", 9);
}

/* ---- state ---- */
static void* g_cam; static int g_camW;
static int g_noclip, g_free, g_playerFrozen;
static float g_pos[3];                 /* noclip player position */
static float g_cpos[3], g_yaw, g_pitch;/* free cam */
static float g_speed = 12.0f;
static float g_home[3]; static int g_follow = 1, g_hidden;
static float g_settlePos[3]; static int g_settle;   /* keep Hob pinned for a few frames after leaving free cam */
static i64 g_qpf, g_last;
static POINT g_mouse; static int g_rmb;
static u8 g_prevKey[256];

static int down(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }
/* mod keys - read from HobTools.ini ([Keys] NOCLIP=F6 ...), written by Hob Keybinds.exe */
static int g_keys[6] = { 0x75, 0x76, 0x77, 0x78, 0x21, 0x22 };
static const char* g_keyNames[6] = { "NOCLIP", "FREECAM", "TELEPORT", "FOLLOW", "SPEEDUP", "SPEEDDOWN" };
#define K_NOCLIP g_keys[0]
#define K_FREECAM g_keys[1]
#define K_TELEPORT g_keys[2]
#define K_FOLLOW g_keys[3]
#define K_SPEEDUP g_keys[4]
#define K_SPEEDDOWN g_keys[5]
static int streq(const char* a, const char* b) { while (*a && *a == *b) a++, b++; return *a == *b; }
static int vk_of(const char* t) {
    static const struct { const char* n; int vk; } T[] = {
        {"SHIFT",0x10},{"CTRL",0x11},{"ALT",0x12},{"SPACE",0x20},{"TAB",0x09},{"ENTER",0x0D},{"UP",0x26},{"DOWN",0x28},{"LEFT",0x25},
        {"RIGHT",0x27},{"TILDA",0xC0},{"END",0x23},{"DEL",0x2E},{"BACKSPACE",0x08},{"CAPSLOCK",0x14},{"PAGEUP",0x21},{"PAGEDOWN",0x22},
        {"HOME",0x24},{"INSERT",0x2D},{"SEMICOLON",0xBA},{"FORWARDSLASH",0xBF},{"LEFTBRACKET",0xDB},{"BACKSLASH",0xDC},
        {"RIGHTBRACKET",0xDD},{"QUOTE",0xDE},{"COMMA",0xBC},{"PERIOD",0xBE},{"MINUS",0xBD},{"PLUS",0xBB},{"ESC",0x1B},
        {"MOUSE_LEFT",0x01},{"MOUSE_RIGHT",0x02},{"MOUSE_MIDDLE",0x04},{"MOUSE_BUTTON1",0x05},{"MOUSE_BUTTON2",0x06} };
    int i;
    if (t[0] && !t[1] && ((t[0] >= 'A' && t[0] <= 'Z') || (t[0] >= '0' && t[0] <= '9'))) return t[0];
    if (t[0] == 'F' && t[1] >= '1' && t[1] <= '9') { int n = t[1] - '0'; if (t[2] >= '0' && t[2] <= '9') n = n * 10 + t[2] - '0'; if (n >= 1 && n <= 12) return 0x6F + n; }
    for (i = 0; i < (int)(sizeof T / sizeof T[0]); i++) if (streq(T[i].n, t)) return T[i].vk;
    return 0;
}
static void load_keys(void) {
    __declspec(dllimport) u32 WINAPI GetFileSize(HANDLE, u32*);
    __declspec(dllimport) BOOL WINAPI ReadFile(HANDLE, void*, u32, u32*, void*);
    __declspec(dllimport) long WINAPI SHGetFolderPathW(HANDLE, int, HANDLE, u32, u16*);
    __declspec(dllimport) HANDLE WINAPI CreateFileW(const u16*, u32, u32, void*, u32, u32, HANDLE);
    /* settings live next to Hob's own settings: Documents\My Games\Runic Games\HOB\HobTools.ini */
    u16 path[600]; const char* tail = "\\My Games\\Runic Games\\HOB\\HobTools.ini"; int k = 0, t = 0;
    path[0] = 0; SHGetFolderPathW(0, 5, 0, 0, path); while (path[k]) k++; while (tail[t] && k < 590) path[k++] = (u16)tail[t++]; path[k] = 0;
    HANDLE h = CreateFileW(path, 0x80000000, 1, 0, 3, 0x80, 0); char buf[2048]; u32 n = 0; int i;
    if (h == (HANDLE)-1) h = CreateFileA("HobTools.ini", 0x80000000, 1, 0, 3, 0x80, 0);   /* old location */
    if (h == (HANDLE)-1) { logs("HobTools.ini not found - using default keys\r\n"); return; }
    ReadFile(h, buf, sizeof buf - 1, &n, 0); CloseHandle(h); buf[n] = 0;
    char* p = buf;
    while (*p) {
        char* line = p; while (*p && *p != '\n') p++; if (*p) *p++ = 0;
        char* e = line; while (*e && *e != '=') e++; if (!*e) continue; *e = 0; char* v = e + 1;
        char* z = v; while (*z && *z != '\r' && *z != ' ' && *z != ';') z++; *z = 0;
        for (i = 0; i < 6; i++) if (streq(line, g_keyNames[i])) { int vk = vk_of(v); if (vk) g_keys[i] = vk; else { logs("unknown key in HobTools.ini: "); logs(v); logs("\r\n"); } }
    }
    logs("keys loaded from HobTools.ini\r\n");
}
static int pressed(int vk) { int d = down(vk), r = d && !g_prevKey[vk]; g_prevKey[vk] = (u8)d; return r; }
static int focused(void) { u32 pid = 0; GetWindowThreadProcessId(GetForegroundWindow(), &pid); return pid == GetCurrentProcessId(); }

#define MAXCAM 16
static void* g_cams[MAXCAM]; static u32 g_camSeen[MAXCAM]; static int g_ncam;
static u8 g_freeSet[MAXCAM];           /* cameras taken over by free cam */
static u32 g_frame;
static int cam_idx(void* c) { int i; for (i = 0; i < g_ncam; i++) if (g_cams[i] == c) return i; return -1; }
static void note_cam(void* cam) {
    int i; if (!cam) return;
    i = cam_idx(cam);
    if (i < 0 && g_ncam < MAXCAM) { i = g_ncam++; g_cams[i] = cam; logx("camera seen: ", (u32)cam); }
    if (i >= 0) g_camSeen[i] = g_frame;
    if (!g_cam) g_cam = cam;
    if (o_getViewport && o_vpWidth) { void* vp = o_getViewport(cam);
        if (vp) { int w = o_vpWidth(vp); if (w >= 400 && !g_camW) { g_cam = cam; g_camW = w; logx("main camera: ", (u32)cam); } } }
}
static int is_free_cam(void* c) { int i = cam_idx(c); return g_free && i >= 0 && g_freeSet[i]; }

/* ---- camera hooks: swallow game writes to the main camera while free cam is on ---- */
static void TC hk_setPos(void* c, const float* v) { note_cam(c); if (is_free_cam(c)) return; o_setPos(c, v); }
static void TC hk_setOri(void* c, const float* q) { note_cam(c); if (is_free_cam(c)) return; o_setOri(c, q); }
static void TC hk_lookAt(void* c, const float* v) { note_cam(c); if (is_free_cam(c)) return; o_lookAt(c, v); }
static void TC hk_yaw(void* c, const float* r) { if (is_free_cam(c)) return; o_yaw(c, r); }
static void TC hk_pitch(void* c, const float* r) { if (is_free_cam(c)) return; o_pitch(c, r); }

/* quaternion helpers, Ogre order w,x,y,z */
static void qmul(const float* a, const float* b, float* o) {
    o[0] = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
    o[1] = a[0]*b[1] + a[1]*b[0] + a[2]*b[3] - a[3]*b[2];
    o[2] = a[0]*b[2] - a[1]*b[3] + a[2]*b[0] + a[3]*b[1];
    o[3] = a[0]*b[3] + a[1]*b[2] - a[2]*b[1] + a[3]*b[0];
}
static void qforward(const float* q, float* f) { /* rotate (0,0,-1) */
    float w = q[0], x = q[1], y = q[2], z = q[3];
    f[0] = -(2*(x*z + w*y)); f[1] = -(2*(y*z - w*x)); f[2] = -(1 - 2*(x*x + y*y));
}
static void yawpitch_q(float yaw, float pitch, float* q) {
    float qy[4] = { cosf_(yaw/2), 0, sinf_(yaw/2), 0 }, qp[4] = { cosf_(pitch/2), sinf_(pitch/2), 0, 0 };
    qmul(qy, qp, q);
}

/* ---- player helpers ---- */
static u8* player(void) {
    if (!g_gameOk) return 0;
    u8* p = (u8*)g_getPlayer();
    if (!p || !p[0x2bc]) return 0;
    return p;
}
static void player_enable(u8* p, int en) {
    void** vt = *(void***)p;
    ((UnitBoolFn)vt[0xa8 / 4])(p, (char)en);   /* SetPlayerEnabled */
    g_flagBD(p, (char)en);
    if (en) g_inputClear(0x100, 0);
}
static void player_setpos(u8* p, const float* v) { void** vt = *(void***)p; ((UnitSetPosFn)vt[0xd4 / 4])(p, v); }
static void player_visible(u8* p, int vis) { void** vt = *(void***)p; ((UnitBoolFn)vt[0xb0 / 4])(p, (char)vis); g_hidden = !vis; }
static void cp3(float* d, const float* s) { d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; }

static void move_input(float* mv, float spd, const float* fwd, const float* right) {
    float f = 0, r = 0, u = 0; int i;
    if (down('W')) f += 1; if (down('S')) f -= 1;
    if (down('D')) r += 1; if (down('A')) r -= 1;
    if (down(0x20) || down('E')) u += 1; if (down(0x11) || down('Q')) u -= 1;
    for (i = 0; i < 3; i++) mv[i] = (fwd[i] * f + right[i] * r) * spd;
    mv[1] += u * spd;
}

/* ---- console click-to-paste ---- */
typedef void (TC *UpdFn)(void*);
typedef void (TC *SetInputFn)(void*, const void*);
static UpdFn o_conUpdate; static SetInputFn g_setInput;
static u8* g_con; static u32 g_conSeen; static int g_conOk;
typedef struct { union { char buf[16]; char* ptr; } u; u32 size, res; } MsStr;
static const char* msstr(const MsStr* s, u32* n) { *n = s->size; return s->res >= 16 ? s->u.ptr : s->u.buf; }
#include "i18n_ht.h"
static const char* T(int id);
static void tfmt(char* out, int cap, int id, const char* a);
static void tfmti(char* out, int cap, int id, int v);
static void mod_descs_refresh(void);
static int g_conWasOpen;
static int console_open(void) { return g_con && (g_frame - g_conSeen) <= 10; }
static void con_style_frame(void);
static void TC hk_conUpdate(void* self) { g_con = (u8*)self; g_conSeen = g_frame; o_conUpdate(self); con_style_frame(); }
static void logi(const char* s, int v) { char b[16]; int i = 15, neg = v < 0; u32 u = neg ? -v : v; b[i--] = 0;
    do { b[i--] = (char)('0' + u % 10); u /= 10; } while (u); if (neg) b[i--] = '-'; logs(s); logs(b + i + 1); logs("\r\n"); }

static void sound(int kind);
typedef void (TC *RefreshFn)(void*); static RefreshFn g_logRefresh;
typedef void (TC *ScrollFn)(void*, int); static ScrollFn g_logScroll;
typedef long (WINAPI *WndProcFn)(HANDLE, u32, u32, long);
__declspec(dllimport) long WINAPI SetWindowLongW(HANDLE, int, long);
__declspec(dllimport) long WINAPI CallWindowProcW(WndProcFn, HANDLE, u32, u32, long);
static HANDLE g_gameWnd; static WndProcFn o_wndProc; static int g_wheel; static int g_conIsOpen;
static long WINAPI hk_wndProc(HANDLE h, u32 msg, u32 wp, long lp) {
    if (msg == 0x20A && g_conIsOpen) { g_wheel += (short)(wp >> 16); return 0; }   /* WM_MOUSEWHEEL: scroll the console */
    return CallWindowProcW(o_wndProc, h, msg, wp, lp);
}
static void console_click(void) {
    POINT m; RECT rc; HANDLE w = GetForegroundWindow();
    u8* lw = *(u8**)(g_con + 0xa0); if (!lw) return;
    GetCursorPos(&m); ScreenToClient(w, &m); GetClientRect(w, &rc);
    float W = (float)(rc.r - rc.l), H = (float)(rc.b - rc.t); if (W < 1 || H < 1) return;
    float fx = m.x / W, fy = m.y / H;
    int size = *(int*)(lw + 0x14), vis = *(int*)(lw + 0x1c), scroll = *(int*)(lw + 0x24);
    if (fx < 0.015f || fx > 0.60f || vis <= 0) return;
    float top = 0.0095f, lh = 0.0200f;                 /* calibrated at 16:9 */
    int row = (int)((fy - top) / lh); if (fy < top || row >= vis) return;
    /* the log is stored newest-first: entry 0 is the bottom line */
    int idx = scroll + (vis - 1 - row); if (idx < 0 || idx >= size) return;
    void** map = *(void***)(lw + 0x8); u32 msz = *(u32*)(lw + 0xc), off = *(u32*)(lw + 0x10);
    const MsStr* line = (const MsStr*)map[(off + idx) & (msz - 1)]; if (!line) return;
    u32 n; const char* t = msstr(line, &n); char out[128]; u32 i = 0, o = 0;
    if (i < n && t[i] == '>') i++;
    while (i < n && o < sizeof(out) - 1) {
        if (t[i] == '^' && i + 1 < n) { i += 2; continue; }      /* colour codes */
        if (t[i] == ' ' && i + 2 < n && t[i + 1] == '-' && t[i + 2] == ' ') break;  /* "NAME - desc" */
        out[o++] = t[i++];
    }
    while (o && out[o - 1] == ' ') o--;
    out[o] = 0;
    if (out[0] == '=' && out[1] == '=') return;                  /* category header */
    if (!o) return;
    MsStr ms; memset(&ms, 0, sizeof ms);
    if (o < 16) { memcpy(ms.u.buf, out, o + 1); ms.res = 15; } else { ms.u.ptr = out; ms.res = o; }
    ms.size = o;
    { MsStr* in = (MsStr*)(g_con + 0x80);           /* clear the input box first */
      if (in->res >= 16) { if (in->u.ptr) in->u.ptr[0] = 0; } else in->u.buf[0] = 0;
      in->size = 0; }
    g_setInput(g_con, &ms);
    sound(0);
}


/* ---- console safety net ----
   Some of Hob's developer commands crash the release build (ReloadUI dereferences a null button,
   LeakTest/LeakWorldTest throw from std::stoi, Crash is meant to crash). They are replaced with a
   message. Commands that parse a number use the game's std::stoi, which throws (= crash) on text
   like "abc"; when that stoi is called from a console command we parse the number ourselves. */
typedef void (TC *LogAddFn)(void*, const MsStr*); static LogAddFn g_logAdd;
static void con_msg(const char* s0) {
    char s[320]; u32 n = 0;                          /* end every line with ^7 so its colour doesn't leak into the next one */
    while (s0[n] && n < sizeof s - 3) { s[n] = s0[n]; n++; } s[n++] = '^'; s[n++] = '7'; s[n] = 0;
    if (!g_con || !g_logAdd) return;
    u8* lw = *(u8**)(g_con + 0xa0); if (!lw) return;
    MsStr ms;
    memset(&ms, 0, sizeof ms);
    if (n < 16) { memcpy(ms.u.buf, s, n + 1); ms.res = 15; } else { ms.u.ptr = (char*)s; ms.res = n; }
    ms.size = n; g_logAdd(lw, &ms);
}
/* GetAllUpgrades in the game just sets every upgrade bit (0xffffffff). That skips Ancient Garb's health
   boost, turns on cut upgrades the shop has no card for, and gives upgrades whose story unlock hasn't
   happened yet - the upgrade shop then shows them as "new" all at once and its list breaks.
   This version does what buying each one would: for every upgrade the shop sells (shop table +0x2c)
   whose unlock is met (same check the shop uses), give it through the game's own function and mark it
   as already seen by the shop (save +0x40148). Cut/locked upgrades a save already has are cleared. */
typedef void (TC *CmdFn)(void*, void*); static CmdFn o_getAll;
typedef void (TC *SetUpgFn)(void*, int);
typedef char (WINAPI *PrereqFn)(const void*);
typedef u8* (*GetSaveFn)(void);
static void TC cmd_getAll(void* self, void* args) {
    u8* p = g_getPlayer ? (u8*)g_getPlayer() : 0; u8* sv; u32 ok = 0; int i, given = 0, later = 0;
    (void)self; (void)args;
    if (!p) { con_msg(T(T_NOPLAYER)); return; }
    for (i = 0; i < 23; i++) {
        u8* e = g_base + g_b->shopTable + i * 0x80;
        if (*(u32*)e != (u32)i || !e[0x2c]) continue;                         /* not sold in the shop */
        if (*(u32*)(e + 0x20) && !((PrereqFn)(g_base + g_b->prereqMet))(e + 0x10)) { later++; continue; }
        ok |= 1u << i;
        if (!(*(u32*)(p + 0x4ec) & (1u << i))) { ((SetUpgFn)(g_base + g_b->setUpgrade))(p, i); given++; }
    }
    *(u32*)(p + 0x4ec) &= ok;                                                  /* drop cut/locked ones */
    sv = ((GetSaveFn)(g_base + g_b->getSave))();
    if (sv) *(u32*)(sv + 0x40148) |= ok;                                       /* shop: already seen */
    con_msg(T(T_GAVEALL));
    if (later) { char m[200]; tfmti(m, sizeof m, T_LATER, later); con_msg(m); }
    (void)given;
}
static void TC cmd_disabled(void* self, void* args) { (void)self; (void)args;
    con_msg(T(T_DISABLED)); }
typedef int (*StoiFn)(const MsStr*); static StoiFn o_stoi; static u8 g_tramp[16];
static int hk_stoi(const MsStr* s) {
    u8* ret = (u8*)__builtin_return_address(0);
    if (ret < g_base + g_b->cmdLo || ret >= g_base + g_b->cmdHi) return o_stoi(s);   /* not a console command */
    u32 n, i = 0, v = 0; int neg = 0, digits = 0; const char* t = msstr(s, &n);
    while (i < n && (t[i] == ' ' || t[i] == '\t')) i++;
    if (i < n && (t[i] == '-' || t[i] == '+')) { neg = t[i] == '-'; i++; }
    while (i < n && t[i] >= '0' && t[i] <= '9') { v = v < 200000000u ? v * 10 + (u32)(t[i] - '0') : 0x7fffffffu; i++; digits++; }
    if (!digits) { con_msg(T(T_NOTNUM)); return 0; }
    if (v > 0x7fffffffu) v = 0x7fffffffu;
    return neg ? -(int)v : (int)v;
}
__declspec(dllimport) BOOL WINAPI IsBadReadPtr(const void*, u32);
static int entry_ok(const u8* q) {           /* does q look like a command-table entry? */
    const MsStr* nm = (const MsStr*)(q + 4); const u8* fn = *(const u8* const*)(q + 0x34);
    if (IsBadReadPtr(q, 0x38)) return 0;
    if (*(const u32*)q > 1) return 0;
    if (fn < g_base + 0x1000 || fn >= g_base + g_b->textEnd) return 0;
    if (nm->res < 15 || nm->size > nm->res || nm->size > 64) return 0;
    if (nm->res >= 16 && IsBadReadPtr(nm->u.ptr, nm->size + 1)) return 0;
    return 1;
}
static int msstr_eq(const MsStr* m, const char* c) {
    u32 n, i; const char* t = msstr(m, &n);
    for (i = 0; i < n; i++) if (!c[i] || t[i] != c[i]) return 0;
    return c[n] == 0;
}
static void fix_getall(u8* q) {
    if (msstr_eq((const MsStr*)(q + 4), "GetAllUpgrades") && *(void**)(q + 0x34) == (void*)(g_base + g_b->getAllUpgrades)
        && sig(g_b->setUpgrade, "8b442404", 4) && sig(g_b->prereqMet, "56e8", 2) && sig(g_b->getSave, "a1", 1)) {
        o_getAll = (CmdFn)(g_base + g_b->getAllUpgrades); *(void**)(q + 0x34) = (void*)cmd_getAll; logs("GetAllUpgrades fixed\r\n"); }
}
static void set_msstr(MsStr* m, const char* s);
static void TC cmd_disabled_keys(void* self, void* args) { (void)self; (void)args; con_msg(T(T_DISABLED_KEYS)); }
static void disable_if_bad(u8* q, int* n) {
    static const char* bad[] = { "Crash", "ReloadUI", "LeakTest", "LeakWorldTest" }; int b;
    for (b = 0; b < 4; b++) if (msstr_eq((const MsStr*)(q + 4), bad[b])) { *(void**)(q + 0x34) = (void*)cmd_disabled; (*n)++; }
    /* ReloadPlayer rebuilds the player's input map wrongly and Hob then saves nonsense key bindings */
    if (msstr_eq((const MsStr*)(q + 4), "ReloadPlayer")) { *(void**)(q + 0x34) = (void*)cmd_disabled_keys; set_msstr((MsStr*)(q + 0x1c), "Disabled by HobTools - it scrambles your key bindings"); (*n)++; }
}

/* ================= shared mod actions (used by the keys and by the console commands) ================= */
static int do_noclip(u8* p) {
    if (!p) return 0;
    g_noclip = !g_noclip;
    if (g_noclip) { const float* gp = g_unitGetPos(p, 0); g_pos[0] = gp[0]; g_pos[1] = gp[1]; g_pos[2] = gp[2]; }
    sound(g_noclip ? 1 : 2); logs(g_noclip ? "noclip ON\r\n" : "noclip OFF\r\n");
    return 1;
}
static int do_freecam(u8* p) {
    if (!g_cam) return 0;
    g_free = !g_free;
    if (g_free) {
        const float* cp = o_getPos(g_cam); const float* q = o_getOri(g_cam); float f[3];
        g_cpos[0] = cp[0]; g_cpos[1] = cp[1]; g_cpos[2] = cp[2];
        qforward(q, f); g_yaw = atan2f_(-f[0], -f[2]); g_pitch = asinf_(f[1]);
        g_rmb = 0;
        if (p) cp3(g_home, g_unitGetPos(p, 0));
        { int i; for (i = 0; i < g_ncam; i++) { g_freeSet[i] = (g_frame - g_camSeen[i]) < 30;
            if (g_freeSet[i]) logx("freecam takes camera: ", (u32)g_cams[i]); } }
    }
    if (!g_free && p) {   /* back to where Hob was (or where F8 put him) */
        player_setpos(p, g_home); if (g_noclip) cp3(g_pos, g_home);
        if (g_hidden) player_visible(p, 1);
        if (!g_noclip) { cp3(g_settlePos, g_home); g_settle = 10; }
    }
    sound(g_free ? 1 : 2); logs(g_free ? "freecam ON\r\n" : "freecam OFF\r\n");
    return 1;
}
static int do_teleport(u8* p) {
    if (!g_free || !p) return 0;
    cp3(g_home, g_cpos); cp3(g_pos, g_cpos);
    if (!g_noclip) { g_noclip = 1; logs("noclip ON (auto, after teleport)\r\n"); }
    sound(1); logs("teleport to camera\r\n");
    return 1;
}
static void do_follow(u8* p) {
    g_follow = !g_follow; sound(g_follow ? 1 : 2);
    if (!g_follow && g_free && p) { player_setpos(p, g_home); if (g_hidden) player_visible(p, 1); }
}

/* ================= console look: colours ================= */
#define C_HEAD "^3"      /* category headers          */
#define C_NAME "^5"      /* command names             */
#define C_TEXT "^7"      /* normal text / descriptions */
#define C_MOD  "^2"      /* HobTools messages          */

typedef struct { u8* begin; u8* end; } ArgVec;          /* std::vector<std::string>, 0x18 per string */
static int arg_count(const void* a) { const ArgVec* v = (const ArgVec*)a; return v ? (int)((v->end - v->begin) / 0x18) : 0; }
static const char* arg_str(const void* a, int i, char* buf, int cap) {
    u32 n; const char* t = msstr((const MsStr*)(((const ArgVec*)a)->begin + i * 0x18), &n); int k;
    for (k = 0; k < (int)n && k < cap - 1; k++) buf[k] = t[k]; buf[k] = 0; return buf;
}
/* first non-empty argument (the game passes an empty string when a command is typed on its own) */
static const char* first_arg(const void* a, char* buf, int cap) {
    int i, n = arg_count(a);
    for (i = 0; i < n; i++) { arg_str(a, i, buf, cap); if (buf[0] && !(buf[0] == ' ' && !buf[1])) return buf; }
    return 0;
}
static char lc(char c) { return (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c; }
static int ieq(const char* a, const char* b) { while (*a && lc(*a) == lc(*b)) a++, b++; return !*a && !*b; }
static int icontains(const char* h, u32 hn, const char* w) {
    u32 i, k, wn = 0; while (w[wn]) wn++; if (!wn) return 1;
    for (i = 0; i + wn <= hn; i++) { for (k = 0; k < wn && lc(h[i + k]) == lc(w[k]); k++); if (k == wn) return 1; }
    return 0;
}
static void sapp(char* d, int cap, const char* s) { int n = 0; while (d[n]) n++; while (*s && n < cap - 1) d[n++] = *s++; d[n] = 0; }
static void sappn(char* d, int cap, const char* s, u32 len) { int n = 0; u32 i; while (d[n]) n++; for (i = 0; i < len && n < cap - 1; i++) d[n++] = s[i]; d[n] = 0; }
static void sappi(char* d, int cap, int v) { char b[12]; int i = 11; b[i] = 0; if (!v) b[--i] = '0'; while (v) { b[--i] = (char)('0' + v % 10); v /= 10; } sapp(d, cap, b + i); }

/* the game's own CRT, so strings we put into the command table can be freed by the game */
typedef void* (__cdecl *MallocFn)(u32); typedef void (__cdecl *FreeFn)(void*);
static MallocFn g_malloc; static FreeFn g_free_;
static void set_msstr(MsStr* m, const char* s) {
    u32 n = 0; while (s[n]) n++;
    if (m->res >= 16 && g_free_) g_free_(m->u.ptr);
    if (n < 16 || !g_malloc) { u32 k = n < 15 ? n : 15; memcpy(m->u.buf, s, k); m->u.buf[k] = 0; m->res = 15; m->size = k; return; }
    m->u.ptr = (char*)g_malloc(n + 1); memcpy(m->u.ptr, s, n + 1); m->res = n; m->size = n;
}

/* ---- game language ---- */
static u8* lang_mgr(void) { u8* f = g_base + g_b->langMgr; if (f[0] != 0xa1 || f[5] != 0xc3) return 0;
    u8* m = **(u8***)(f + 1); return (m && !IsBadReadPtr(m, 0x78)) ? m : 0; }
static const MsStr* lang_name(int i) {      /* i<0: current language */
    u8* m = lang_mgr(); u8 *nb, *ne; u32 idx;
    if (!m) return 0; nb = *(u8**)(m + 0x70); ne = *(u8**)(m + 0x74);
    if (i < 0) { u32* vb = *(u32**)(m + 4); if (!vb || vb == *(u32**)(m + 8) || IsBadReadPtr(vb, 4)) return 0; idx = *vb; } else idx = (u32)i;
    if (!nb || ne < nb || idx >= (u32)((ne - nb) / 0x18) || IsBadReadPtr(nb + idx * 0x18, 0x18)) return 0;
    return (const MsStr*)(nb + idx * 0x18); }
/* which of our translations fits the game's language (LANGUAGE setting, e.g. "FRENCH").
   The console font only has plain A-Z, so non-Latin languages use English. */
static int lhas(const char* s, u32 n, const char* w) { u32 i, k; for (i = 0; i < n; i++) { for (k = 0; w[k] && i + k < n && (s[i + k] & ~32) == (w[k] & ~32); k++); if (!w[k]) return 1; } return 0; }
static int lang_of(const char* s, u32 n) {
    if (lhas(s, n, "BRAZ") || (lhas(s, n, "PORT") && lhas(s, n, "BR"))) return L_PT_BR;
    if (lhas(s, n, "PORT")) return L_PT;
    if (lhas(s, n, "FRENCH") || lhas(s, n, "FRAN")) return L_FR;
    if (lhas(s, n, "GERMAN") || lhas(s, n, "DEUT")) return L_DE;
    if (lhas(s, n, "SPAN") || lhas(s, n, "ESPA")) return L_ES;
    if (lhas(s, n, "ITAL")) return L_IT;
    if (lhas(s, n, "DANISH") || lhas(s, n, "DANSK")) return L_DA;
    if (lhas(s, n, "DUTCH") || lhas(s, n, "NEDER") || lhas(s, n, "NETHER")) return L_NL;
    if (lhas(s, n, "FINN") || lhas(s, n, "SUOMI")) return L_FI;
    if (lhas(s, n, "NORW") || lhas(s, n, "NORSK")) return L_NO;
    if (lhas(s, n, "SWED") || lhas(s, n, "SVENSK")) return L_SV;
    if (lhas(s, n, "POLI") || lhas(s, n, "POLSK")) return L_PL;
    if (lhas(s, n, "TURK")) return L_TR;
    return L_EN;
}
static int g_lang = -1;
#ifndef HOBTOOLS_LANGUAGES
static int cur_lang(void) { return L_EN; }   /* 1.0: English only - languages come in a later version */
#else
static int cur_lang(void) { const MsStr* m = lang_name(-1); u32 n; const char* c; if (!m) return L_EN; c = msstr(m, &n); return lang_of(c, n); }
#endif
static const char* T(int id) { int l = cur_lang(); const char* s = g_tr[l][id]; return s ? s : g_tr[L_EN][id]; }
static void tfmt(char* out, int cap, int id, const char* a) {
    const char* t = T(id); int n = 0;
    while (*t && n < cap - 1) { if (t[0] == '{' && t[1] == '1' && t[2] == '}') { const char* q = a; while (*q && n < cap - 1) out[n++] = *q++; t += 3; } else out[n++] = *t++; }
    out[n] = 0; }
static void tfmti(char* out, int cap, int id, int v) { char b[12]; int i = 11, neg = v < 0; u32 u = neg ? -v : v; b[i] = 0; do { b[--i] = (char)('0' + u % 10); u /= 10; } while (u); if (neg) b[--i] = '-'; tfmt(out, cap, id, b + i); }

/* ================= categories for help ================= */
typedef struct { const char* key; int title; int blurb; const char* names; } Cat;
static const Cat g_cats[] = {
  { "mod", T_CAT_MOD_T, T_CAT_MOD_B,
    " noclip freecam teleport followcam flyspeed " },
  { "player", T_CAT_PLAYER_T, T_CAT_PLAYER_B,
    " god invincible healme hurt killme killenergy healthupgrade energyupgrade setplayervisible targetable reloadplayer playerinfo playersphere movetomouse warp " },
  { "items", T_CAT_ITEMS_T, T_CAT_ITEMS_B,
    " accessories allaccessories noglove upgradesword swordpiece wardrobe showupgrades getupgrade getallupgrades removeupgrade removeallupgrades giveitem givetoken list setlist " },
  { "world", T_CAT_WORLD_T, T_CAT_WORLD_B,
    " map mapedit mapsave fasttravelall worldpieces worldchunks layout fow treefade killall monster setvar cells " },
  { "progress", T_CAT_PROGRESS_T, T_CAT_PROGRESS_B,
    " questgive questcomplete questshowactive questshowcomplete giveachievement giveallachievements resetachievements timeplayed savegame " },
  { "camera", T_CAT_CAMERA_T, T_CAT_CAMERA_B,
    " cammovedist camzoom resetcamera dof lighting fogmode tonemapper volumetricsmode posting toggleculling particlecull frustum shownormals hidenormals setmousestrat togglemousestickui togglemenu " },
  { "console", T_CAT_CONSOLE_T, T_CAT_CONSOLE_B,
    " help clear " },
  { "debug", T_CAT_DEBUG_T, T_CAT_DEBUG_B,
    0 },   /* everything else */
};
#define NCATS ((int)(sizeof g_cats / sizeof g_cats[0]))
static int cat_of(const MsStr* name) {
    u32 n, i; const char* t = msstr(name, &n); char key[40]; int c;
    if (n == 0 || n > 36) return NCATS - 1;
    key[0] = ' '; for (i = 0; i < n; i++) key[i + 1] = lc(t[i]); key[n + 1] = ' '; key[n + 2] = 0;
    for (c = 0; c < NCATS - 1; c++) { const char* h = g_cats[c].names; u32 hn = 0; while (h[hn]) hn++;
        if (icontains(h, hn, key)) return c; }
    return NCATS - 1;
}

/* the command table, collected once */
static u8* g_cmd[256]; static int g_ncmd;
static int entry_ok(const u8* q);
static void cmd_line(u8* e) {
    char line[240] = ">" C_NAME; u32 n; const char* t;
    t = msstr((const MsStr*)(e + 4), &n); sappn(line, sizeof line, t, n);
    sapp(line, sizeof line, C_TEXT " - ");
    t = msstr((const MsStr*)(e + 0x1c), &n); sappn(line, sizeof line, t, n);
    con_msg(line);
}
static int name_less(u8* a, u8* b) {
    u32 na, nb, i; const char* x = msstr((const MsStr*)(a + 4), &na); const char* y = msstr((const MsStr*)(b + 4), &nb);
    for (i = 0; i < na && i < nb; i++) { char p = lc(x[i]), q = lc(y[i]); if (p != q) return p < q; }
    return na < nb;
}
static int list_cat(int c, const char* word) {   /* prints matching commands of category c (sorted); returns count */
    u8* sel[256]; int n = 0, i, j;
    for (i = 0; i < g_ncmd; i++) {
        u8* e = g_cmd[i]; u32 nn, dn; const char* nm = msstr((const MsStr*)(e + 4), &nn); const char* ds = msstr((const MsStr*)(e + 0x1c), &dn);
        if (cat_of((const MsStr*)(e + 4)) != c) continue;
        if (word && !icontains(nm, nn, word) && !icontains(ds, dn, word)) continue;
        sel[n++] = e;
    }
    for (i = 1; i < n; i++) { u8* k = sel[i]; j = i - 1; while (j >= 0 && name_less(k, sel[j])) { sel[j + 1] = sel[j]; j--; } sel[j + 1] = k; }
    if (n) { char h[96] = C_HEAD "== "; sapp(h, sizeof h, T(g_cats[c].title)); sapp(h, sizeof h, " =="); con_msg(h); }
    for (i = 0; i < n; i++) cmd_line(sel[i]);
    return n;
}
static void TC cmd_help(void* self, void* args) {
    char w[40]; int c, total = 0; (void)self; mod_descs_refresh();
    if (first_arg(args, w, sizeof w)) {
        for (c = 0; c < NCATS; c++) if (ieq(w, g_cats[c].key)) { list_cat(c, 0); return; }
        if (ieq(w, "all")) { for (c = 0; c < NCATS; c++) list_cat(c, 0); return; }
        for (c = 0; c < NCATS; c++) total += list_cat(c, w);
        if (!total) { char m[200] = C_TEXT; tfmt(m + 2, sizeof m - 2, T_NOMATCH, w); con_msg(m); }
        return;
    }
    { char m[200] = C_HEAD; sapp(m, sizeof m, T(T_TITLE)); con_msg(m); }
    { char m[200] = C_TEXT; tfmti(m + 2, sizeof m - 2, T_COUNT, g_ncmd); con_msg(m); }
    { char m[200] = C_TEXT; sapp(m, sizeof m, T(T_HINT)); con_msg(m); }
    for (c = 0; c < NCATS; c++) {
        char m[240] = ">" C_NAME "help "; int k = 0, i;
        for (i = 0; i < g_ncmd; i++) if (cat_of((const MsStr*)(g_cmd[i] + 4)) == c) k++;
        sapp(m, sizeof m, g_cats[c].key); sapp(m, sizeof m, C_TEXT " - "); sapp(m, sizeof m, T(g_cats[c].title));
        sapp(m, sizeof m, ": "); sapp(m, sizeof m, T(g_cats[c].blurb)); sapp(m, sizeof m, " ("); sappi(m, sizeof m, k); sapp(m, sizeof m, ")");
        con_msg(m);
    }
}

/* ================= HobTools console commands ================= */
static void say_onoff(int what, int on) { char m[120] = C_MOD; sapp(m, sizeof m, T(what)); sapp(m, sizeof m, " "); sapp(m, sizeof m, T(on ? T_ON : T_OFF)); con_msg(m); }
static void mod_msg(int id) { char m[240] = C_MOD; sapp(m, sizeof m, T(id)); con_msg(m); }
static void TC cmd_noclip(void* self, void* args) { (void)self; (void)args;
    if (!do_noclip(player())) mod_msg(T_NOCLIP_NOGAME); else say_onoff(T_NOCLIP, g_noclip); }
static void TC cmd_freecam(void* self, void* args) { (void)self; (void)args;
    if (!do_freecam(player())) mod_msg(T_FREECAM_NOTREADY); else {
        say_onoff(T_FREECAM, g_free); if (g_free) mod_msg(T_FREECAM_HOWTO); } }
static void TC cmd_teleport(void* self, void* args) { (void)self; (void)args;
    if (!do_teleport(player())) mod_msg(T_TP_FIRST);
    else mod_msg(T_TP_OK); }
static void TC cmd_follow(void* self, void* args) { (void)self; (void)args;
    do_follow(player()); say_onoff(T_FOLLOW, g_follow); }
static void TC cmd_flyspeed(void* self, void* args) {
    char w[24], m[200] = C_MOD, num[16]; (void)self;
    if (first_arg(args, w, sizeof w)) {
        const char* s = w; float v = 0, f = 0.1f; int dot = 0, any = 0;
        for (; *s; s++) { if (*s >= '0' && *s <= '9') { any = 1; if (dot) { v += (*s - '0') * f; f *= 0.1f; } else v = v * 10 + (*s - '0'); } else if ((*s == '.' || *s == ',') && !dot) dot = 1; else break; }
        if (!any || v <= 0) { mod_msg(T_SPEED_USAGE); return; }
        if (v > 50) v = 50; g_speed = 12.0f * v;
    }
    { float x = g_speed / 12.0f; int i = (int)x, d = (int)((x - i) * 10 + 0.5f); if (d == 10) { i++; d = 0; }
      num[0] = 0; sappi(num, sizeof num, i); sapp(num, sizeof num, "."); sappi(num, sizeof num, d);
      tfmt(m + 2, sizeof m - 2, T_SPEED, num); con_msg(m); }
}

typedef struct { const char* old; const char* name; int desc; void* fn; } ModCmd;
static const ModCmd g_modcmds[] = {
    { "Crash",         "Noclip", T_D_NOCLIP,           (void*)cmd_noclip },
    { "ReloadUI",      "FreeCam", T_D_FREECAM, (void*)cmd_freecam },
    { "LeakTest",      "Teleport", T_D_TELEPORT,         (void*)cmd_teleport },
    { "LeakWorldTest", "FollowCam", T_D_FOLLOW,       (void*)cmd_follow },
    { "NumberTest",    "FlySpeed", T_D_FLYSPEED,           (void*)cmd_flyspeed },
};
static u8* g_modEntry[8]; static int g_modLang = L_EN;
static void mod_descs_refresh(void) {      /* mod command descriptions follow the game language */
    int l = cur_lang(), k; if (l == g_modLang) return; g_modLang = l;
    for (k = 0; k < (int)(sizeof g_modcmds / sizeof g_modcmds[0]); k++) if (g_modEntry[k]) set_msstr((MsStr*)(g_modEntry[k] + 0x1c), T(g_modcmds[k].desc));
}
static void console_safety(void) {
    /* command table: static array of { u32 flag; std::string name; std::string desc; void* fn; } (0x38 bytes) */
    u8* e = g_base + g_b->cmdTable; u8* q; int n = 0;
    if (sig(g_b->logAdd, "538bd956ff7424", 7)) g_logAdd = (LogAddFn)(g_base + g_b->logAdd);
    if (!entry_ok(e) || !msstr_eq((const MsStr*)(e + 4), "LeakTest")) logs("console safety: command table not found\r\n");
    else {
        { HANDLE crt = GetModuleHandleA("msvcr110.dll");
          if (crt) { g_malloc = (MallocFn)GetProcAddress(crt, "malloc"); g_free_ = (FreeFn)GetProcAddress(crt, "free"); } }
        while (entry_ok(e - 0x38)) e -= 0x38;                                    /* back to the start of the table */
        for (q = e; entry_ok(q) && g_ncmd < 256; q += 0x38) {
            int k;
            fix_getall(q);
            for (k = 0; k < (int)(sizeof g_modcmds / sizeof g_modcmds[0]); k++)
                if (msstr_eq((const MsStr*)(q + 4), g_modcmds[k].old)) {      /* crashing/useless dev command -> mod command */
                    set_msstr((MsStr*)(q + 4), g_modcmds[k].name); set_msstr((MsStr*)(q + 0x1c), g_tr[L_EN][g_modcmds[k].desc]); g_modEntry[k] = q;
                    *(u32*)q = 0; *(void**)(q + 0x34) = g_modcmds[k].fn; n++; }
            disable_if_bad(q, &n);                                                 /* anything left that crashes */
            if (msstr_eq((const MsStr*)(q + 4), "Help")) *(void**)(q + 0x34) = (void*)cmd_help;
            g_cmd[g_ncmd++] = q;
        }
    }
    logi("console: mod commands added / crashing commands replaced: ", n); logi("console: commands ", g_ncmd);
    if (sig(g_b->stoi, "558bec64a100000000", 9)) {               /* detour the game's std::stoi */
        u8* f = g_base + g_b->stoi; u32 old; int rel;
        memcpy(g_tramp, f, 9); g_tramp[9] = 0xe9; rel = (int)(f + 9) - (int)(g_tramp + 14); memcpy(g_tramp + 10, &rel, 4);
        VirtualProtect(g_tramp, 16, 0x40, &old);
        o_stoi = (StoiFn)g_tramp;
        if (VirtualProtect(f, 5, 0x40, &old)) { rel = (int)(u8*)hk_stoi - (int)(f + 5); f[0] = 0xe9; memcpy(f + 1, &rel, 4); VirtualProtect(f, 5, old, &old); logs("console safety: number parsing guarded\r\n"); }
    }
}
/* ---- game sounds (FMOD Studio). The game's own UI events are looked up by name once. ---- */
typedef int (WINAPI *FUpd)(void*);
typedef int (WINAPI *FGetCount)(void*, int*);
typedef int (WINAPI *FGetList)(void*, void**, int, int*);
typedef int (WINAPI *FGetPath)(void*, char*, int, int*);
typedef int (WINAPI *FCreate)(void*, void**);
typedef int (WINAPI *FInst)(void*);
static FUpd o_fUpdate; static FGetCount f_bankCount, f_evCount; static FGetList f_bankList, f_evList; static FGetPath f_getPath;
static FCreate f_create; static FInst f_start, f_release;
typedef int (WINAPI *FIs3D)(void*, unsigned char*);
typedef int (WINAPI *FGetLis)(void*, int, float*);
typedef int (WINAPI *FSet3D)(void*, const float*);
static FIs3D f_is3D; static FGetLis f_getLis; static FSet3D f_set3D;
static void* g_fsys; static int g_sndScanned; static void* g_sndClick; static void* g_sndOn; static void* g_sndOff;
static int WINAPI hk_fUpdate(void* sys) { g_fsys = sys; return o_fUpdate(sys); }
static int ends_with(const char* s, const char* t) { int a = 0, b = 0; while (s[a]) a++; while (t[b]) b++; if (b > a) return 0; s += a - b; while (*t) if (*s++ != *t++) return 0; return 1; }
static int has(const char* s, const char* t) { for (; *s; s++) { const char* a = s; const char* b = t; while (*a && *a == *b) a++, b++; if (!*b) return 1; } return 0; }
static void scan_sounds(void) {
    static void* banks[64]; static void* evs[4096]; int nb = 0, i, j, ne;
    int first = !g_sndScanned; g_sndScanned++;
    if (!g_fsys || !f_bankCount || !f_bankList || !f_evList || !f_getPath) { logs("sounds: FMOD not available, using beeps\r\n"); return; }
    if (f_bankList(g_fsys, banks, 64, &nb) != 0) nb = 0;
    for (i = 0; i < nb; i++) {
        ne = 0; if (f_evList(banks[i], evs, 4096, &ne) != 0) continue;
        for (j = 0; j < ne; j++) { char path[256]; int got = 0; if (f_getPath(evs[j], path, sizeof path, &got) != 0) continue;
            if (first && (has(path, "IngameUI") || has(path, "/UI/") || has(path, "Menu"))) { logs("sound: "); logs(path); logs("\r\n"); }
            if (ends_with(path, "/UI/Select")) g_sndClick = evs[j];
            if (has(path, "IngameUI")) {
                if (ends_with(path, "Positive")) g_sndOn = evs[j];
                else if (ends_with(path, "Negative")) g_sndOff = evs[j];
            } } }
    if (!g_sndClick) g_sndClick = g_sndOn;
    if (first || g_sndClick) logs(g_sndClick ? "sounds: using the game's UI sounds\r\n" : "sounds: UI sounds not found yet, using beeps\r\n");
}
/* kind: 0 = click, 1 = on, 2 = off */
static void sound(int kind) {
    if (g_fsys && (!g_sndScanned || (!g_sndClick && g_sndScanned < 5))) scan_sounds();   /* retry a few times: banks load per area */
    void* d = kind == 0 ? g_sndClick : kind == 1 ? (g_sndOn ? g_sndOn : g_sndClick) : (g_sndOff ? g_sndOff : g_sndClick);
    void* inst = 0;
    if (d && f_create && f_start && f_release && f_create(d, &inst) == 0 && inst) {
        /* 3D events would play at the world origin, far from the camera: put them on the listener */
        unsigned char is3d = 0; float a[12];
        if (f_is3D && f_getLis && f_set3D && f_is3D(d, &is3d) == 0 && is3d && f_getLis(g_fsys, 0, a) == 0) f_set3D(inst, a);
        f_start(inst); f_release(inst); return; }
    Beep(kind == 2 ? 440 : 880, 60);
}

static void tick(void) {
    i64 now; g_frame++; float dt; QueryPerformanceCounter(&now);
    dt = g_last ? (float)(double)(now - g_last) / (float)(double)g_qpf : 0; g_last = now;
    if (dt > 0.1f) dt = 0.1f; if (dt < 0) dt = 0;
    int foc = focused();
    u8* p = player();
    /* while the dev console is open: freeze Hob and ignore mod keys / fly keys, so typing doesn't move anything */
    int con = g_conOk && console_open(), act = foc && !con;

    if (act && pressed(K_NOCLIP) && !do_noclip(p)) { Beep(220, 120); logs("noclip: player not available\r\n"); }
    if (act && pressed(K_FREECAM) && !do_freecam(p)) { Beep(220, 120); logs("freecam: no camera yet\r\n"); }
    if (act && g_free && p && pressed(K_TELEPORT)) do_teleport(p);
    if (act && pressed(K_FOLLOW)) do_follow(p);
    if (foc && g_conOk) { HANDLE w = GetForegroundWindow();                 /* hook the game window once for the mouse wheel */
        if (w && w != g_gameWnd) { g_gameWnd = w; o_wndProc = (WndProcFn)SetWindowLongW(w, -4, (long)hk_wndProc); logs("window hooked for mouse wheel\r\n"); } }
    { int open = g_conOk && console_open(); g_conIsOpen = open;
      if (!open) g_wheel = 0;
      if (open && g_logScroll && (g_wheel >= 120 || g_wheel <= -120)) {
          u8* lw = *(u8**)(g_con + 0xa0); int steps = g_wheel / 120; g_wheel -= steps * 120;
          if (lw) g_logScroll(lw, steps * 3); }
      if (open != g_conWasOpen) { logs(open ? "console open\r\n" : "console closed\r\n"); g_conWasOpen = open;
          }
      /* bit 0 = clicked since last call, so even very short clicks are caught */
      short st = GetAsyncKeyState(0x01);
      if (foc && open && (st & 1)) console_click(); }
    if (act && pressed(K_SPEEDUP)) g_speed *= 1.5f;
    if (act && pressed(K_SPEEDDOWN)) g_speed /= 1.5f;
    if (!p) { g_noclip = 0; g_playerFrozen = 0; g_hidden = 0; }

    /* freeze the player's own movement while noclip, freecam or the dev console is on */
    int wantFrozen = p && (g_noclip || g_free || con);
    if (p && wantFrozen != g_playerFrozen) {
        if (!wantFrozen && g_noclip == 0) player_setpos(p, g_settle ? g_settlePos : g_unitGetPos(p, 0));
        player_enable(p, !wantFrozen); g_playerFrozen = wantFrozen;
    }
    /* the game updates Hob's position a frame or two late; re-apply the home position briefly so
       the old free-cam position (high in the air) can't win and drop him to his death */
    if (g_settle && p && !g_free && !g_noclip) { player_setpos(p, g_settlePos); g_settle--; }
    else if (!p || g_free || g_noclip) g_settle = 0;
    float spd = g_speed * dt * ((act && down(0x10)) ? 4.0f : 1.0f);

    if (g_free && g_cam) {
        if (act) {
            float look = 1.6f * dt;
            if (down(0x25)) g_yaw += look; if (down(0x27)) g_yaw -= look;
            if (down(0x26)) g_pitch += look; if (down(0x28)) g_pitch -= look;
            if (down(0x02)) { POINT m; GetCursorPos(&m);
                if (g_rmb) { g_yaw -= (m.x - g_mouse.x) * 0.004f; g_pitch -= (m.y - g_mouse.y) * 0.004f; }
                g_mouse = m; g_rmb = 1; } else g_rmb = 0;
            if (g_pitch > 1.55f) g_pitch = 1.55f; if (g_pitch < -1.55f) g_pitch = -1.55f;
            float cy = cosf_(g_yaw), sy = sinf_(g_yaw), cp = cosf_(g_pitch), sp = sinf_(g_pitch);
            float fwd[3] = { -sy * cp, sp, -cy * cp }, right[3] = { cy, 0, -sy }, mv[3]; int i;
            move_input(mv, spd, fwd, right);
            for (i = 0; i < 3; i++) g_cpos[i] += mv[i];
        }
        float q[4]; yawpitch_q(g_yaw, g_pitch, q);
        { int i; for (i = 0; i < g_ncam; i++) if (g_freeSet[i]) { o_setPos(g_cams[i], g_cpos); o_setOri(g_cams[i], q); } }
        if (p && g_follow) {      /* follow across the map only - keep Hob at his own height so the game never sees a long fall */
            float fp[3]; fp[0] = g_cpos[0]; fp[1] = g_home[1]; fp[2] = g_cpos[2];
            if (!g_hidden) player_visible(p, 0); player_setpos(p, fp); }
    }
    if (g_noclip && p) {
        if (act && !g_free) {
            float fwd[3] = { 0, 0, -1 }, right[3], mv[3], len; int i;
            if (g_cam) { qforward(o_getDerOri(g_cam), fwd); }
            fwd[1] = 0; len = sqrtf_(fwd[0]*fwd[0] + fwd[2]*fwd[2]);
            if (len < 1e-4f) { fwd[0] = 0; fwd[2] = -1; len = 1; }
            fwd[0] /= len; fwd[2] /= len;
            right[0] = -fwd[2]; right[1] = 0; right[2] = fwd[0];
            move_input(mv, spd, fwd, right);
            for (i = 0; i < 3; i++) g_pos[i] += mv[i];
        }
        if (!(g_free && g_follow)) player_setpos(p, g_pos);
    }
}

#ifdef HOBTOOLS_LANGUAGES   /* not part of 0.9: writes the game language into HobTools.ini for Hob Keybinds */
/* Hob Keybinds shows itself in the game's language: keep LANGUAGE=<game language> in HobTools.ini */
__declspec(dllimport) long WINAPI SHGetFolderPathW(HANDLE, int, HANDLE, u32, u16*);
__declspec(dllimport) HANDLE WINAPI CreateFileW(const u16*, u32, u32, void*, u32, u32, HANDLE);
__declspec(dllimport) BOOL WINAPI ReadFile(HANDLE, void*, u32, u32*, void*);
static char g_iniLang[32]; static u32 g_langCheck;
static void ini_lang_sync(void) {
    const MsStr* m = lang_name(-1); u32 n, i, rd = 0, w; const char* c; u16 path[600]; const char* tail = "\\My Games\\Runic Games\\HOB\\HobTools.ini";
    static char buf[4096], out[4200]; int k = 0, t = 0, o = 0; HANDLE h;
    if (!m) return; c = msstr(m, &n); if (!n || n > 30) return;
    for (i = 0; i < n; i++) if (g_iniLang[i] != c[i]) break;
    if (i == n && !g_iniLang[n]) return;                                   /* unchanged */
    for (i = 0; i < n; i++) g_iniLang[i] = c[i]; g_iniLang[n] = 0;
    path[0] = 0; SHGetFolderPathW(0, 5, 0, 0, path); while (path[k]) k++; while (tail[t] && k < 590) path[k++] = (u16)tail[t++]; path[k] = 0;
    h = CreateFileW(path, 0x80000000, 1, 0, 3, 0x80, 0); if (h == (HANDLE)-1) return;
    ReadFile(h, buf, sizeof buf - 1, &rd, 0); CloseHandle(h); buf[rd] = 0;
    { char* p = buf; int have = 0;
      while (*p) { char* line = p; int len; while (*p && *p != '\n') p++; len = (int)(p - line); if (*p) p++;
          if (len >= 9 && !memcmp_(line, "LANGUAGE=", 9)) {
              if ((u32)(len - 9) >= n && !memcmp_(line + 9, c, n) && (len - 9 == (int)n || line[9 + n] == '\r')) have = 1;
              continue; }
          if (o + len + 1 < (int)sizeof out) { memcpy(out + o, line, len); o += len; out[o++] = '\n'; } }
      if (have) return;
      if (o && out[o - 1] == '\n' && o >= 2 && out[o - 2] != '\r') { out[o - 1] = '\r'; out[o++] = '\n'; }
      if (o + 13 + (int)n < (int)sizeof out) { memcpy(out + o, "LANGUAGE=", 9); o += 9; memcpy(out + o, c, n); o += n; out[o++] = '\r'; out[o++] = '\n'; } }
    h = CreateFileW(path, 0x40000000, 0, 0, 2, 0x80, 0); if (h == (HANDLE)-1) return;
    WriteFile(h, out, o, &w, 0); CloseHandle(h); logs("HobTools.ini: language "); logs(g_iniLang); logs("\r\n");
}
#endif
static char TC hk_fireStarted(void* root) {
    tick();
#ifdef HOBTOOLS_LANGUAGES
    if (g_frame - g_langCheck > 180) { g_langCheck = g_frame; ini_lang_sync(); }
#endif
    return o_fireStarted(root);
}

/* ---- IAT patching in HOB.exe ---- */
static int patch_iat(void* target, void* hook) {
    u8* b = g_base; u32 pe = *(u32*)(b + 0x3c);
    u32 iatRva = *(u32*)(b + pe + 24 + 96 + 12 * 8), iatSz = *(u32*)(b + pe + 24 + 96 + 12 * 8 + 4);
    void** s = (void**)(b + iatRva); u32 i, n = iatSz / 4; int hits = 0;
    for (i = 0; i < n; i++) if (s[i] == target) {
        u32 old; VirtualProtect(&s[i], 4, 0x04, &old); s[i] = hook; VirtualProtect(&s[i], 4, old, &old); hits++;
    }
    return hits;
}
static void* ogre(HANDLE m, const char* n) { void* p = GetProcAddress(m, n); if (!p) { logs("missing: "); logs(n); logs("\r\n"); } return p; }


/* ================= console look: layout inspection ================= */
__declspec(dllimport) HANDLE WINAPI LoadLibraryA(const char*);
typedef const MsStr* (TC *OStrFn)(void*);
typedef float (TC *OFltFn)(void*);
typedef float (TC *OFltBFn)(void*, char);
typedef char (TC *OBoolBFn)(void*, char);
typedef int (TC *OIntFn)(void*);
typedef const float* (TC *OColFn)(void*);
static OStrFn oe_getName, oe_getMat, oe_getCaption, ta_getFont; static OFltFn oe_left, oe_top, oe_width, oe_height, ta_charH;
static OFltBFn oe_getAlpha; static OBoolBFn oe_isVis; static OIntFn oe_metrics; static PtrGetFn oe_parent; static OColFn oe_colour;
static u8* g_ovl; static u32 g_ovlSize;
static void* vt_panel, *vt_border, *vt_text, *vt_cont, *vt_elem;
static int g_styleInit, g_styleDumped; static u32 g_styleOpenFrame;
static void logms(const MsStr* m) { u32 n; const char* c; char b[96]; u32 i;
    if (!m || IsBadReadPtr(m, sizeof *m)) { logs("?"); return; }
    if (m->size > 80 || (m->res >= 16 && IsBadReadPtr(m->u.ptr, m->size + 1))) { logs("?"); return; }
    c = msstr(m, &n); for (i = 0; i < n && i < 90; i++) b[i] = c[i]; b[i] = 0; logs(b); }
static void* ovl(const char* n) { HANDLE h = GetModuleHandleA("OgreOverlay.dll"); return h ? GetProcAddress(h, n) : 0; }
static void style_init(void) {
    HANDLE h = GetModuleHandleA("OgreOverlay.dll"); if (!h) return;
    oe_getName = (OStrFn)ovl("?getName@OverlayElement@Ogre@@QBEABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
    oe_getMat = (OStrFn)ovl("?getMaterialName@OverlayElement@Ogre@@UBEABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
    oe_getCaption = (OStrFn)ovl("?getCaption@OverlayElement@Ogre@@UBEABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
    ta_getFont = (OStrFn)ovl("?getFontName@TextAreaOverlayElement@Ogre@@QBEABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@XZ");
    ta_charH = (OFltFn)ovl("?getCharHeight@TextAreaOverlayElement@Ogre@@QBEMXZ");
    oe_left = (OFltFn)ovl("?getLeft@OverlayElement@Ogre@@QBEMXZ"); oe_top = (OFltFn)ovl("?getTop@OverlayElement@Ogre@@QBEMXZ");
    oe_width = (OFltFn)ovl("?getWidth@OverlayElement@Ogre@@QBEMXZ"); oe_height = (OFltFn)ovl("?getHeight@OverlayElement@Ogre@@QBEMXZ");
    oe_getAlpha = (OFltBFn)ovl("?getAlpha@OverlayElement@Ogre@@QAEM_N@Z"); oe_isVis = (OBoolBFn)ovl("?isVisible@OverlayElement@Ogre@@QBE_N_N@Z");
    oe_metrics = (OIntFn)ovl("?getMetricsMode@OverlayElement@Ogre@@UBE?AW4GuiMetricsMode@2@XZ");
    oe_parent = (PtrGetFn)ovl("?getParent@OverlayElement@Ogre@@QAEPAVOverlayContainer@2@XZ");
    oe_colour = (OColFn)ovl("?getColour@OverlayElement@Ogre@@UBEABVColourValue@2@XZ");
    vt_panel = ovl("??_7PanelOverlayElement@Ogre@@6BStringInterface@1@@"); vt_border = ovl("??_7BorderPanelOverlayElement@Ogre@@6BStringInterface@1@@");
    vt_text = ovl("??_7TextAreaOverlayElement@Ogre@@6BStringInterface@1@@"); vt_cont = ovl("??_7OverlayContainer@Ogre@@6BStringInterface@1@@");
    vt_elem = ovl("??_7OverlayElement@Ogre@@6BStringInterface@1@@");
    g_ovl = (u8*)h; g_ovlSize = *(u32*)(g_ovl + *(u32*)(g_ovl + 0x3c) + 0x50);
    g_styleInit = 1;
}
static const char* vt_kind(void* o) { void* v = *(void**)o;
    if (v == vt_panel) return "Panel"; if (v == vt_border) return "BorderPanel"; if (v == vt_text) return "TextArea";
    if (v == vt_cont) return "Container"; if (v == vt_elem) return "Element";
    if ((u8*)v >= g_base && (u8*)v < g_base + 0x5000000) return "HOB"; return "other"; }
typedef void (TC *OColSetFn)(void*, const float*);
typedef void (TC *OVoidFn)(void*);
static OColSetFn pn_setColour; static OVoidFn oe_hide;
static void* map_find(u8* m, const char* name) {     /* layout name->value unordered_map (list head at +0) */
    u8 *head, *n; int c = 0; if (IsBadReadPtr(m, 8)) return 0; head = *(u8**)m; if (!head || IsBadReadPtr(head, 8)) return 0;
    for (n = *(u8**)head; n && n != head && c < 64; n = *(u8**)n, c++) {
        if (IsBadReadPtr(n, 0x24)) return 0;
        if (msstr_eq((MsStr*)(n + 8), name)) return *(void**)(n + 0x20);
    }
    return 0; }
static int is_ovl_elem(void* p) { void* v; if (!p || ((u32)p & 3) || IsBadReadPtr(p, 0x40)) return 0; v = *(void**)p;
    return v == vt_panel || v == vt_border || v == vt_text || v == vt_cont; }
static void tint(void* e, float r, float g, float b, float a) {
    float c[4] = { r, g, b, a }; const float* cur;
    if (!e || *(void**)e != vt_panel || !pn_setColour) return;
    cur = oe_colour(e); if (cur && cur[0] == r && cur[1] == g && cur[2] == b && cur[3] == a) return;
    pn_setColour(e, c); }
static int g_btnLogged;
static void hide_button(void* btn, const char* nm) {    /* game button object: its overlay element is at +0xc */
    void* p;
    if (!btn || IsBadReadPtr(btn, 0x10)) return;
    p = *(void**)((u8*)btn + 0xc);
    if (!is_ovl_elem(p)) { if (!g_btnLogged) { logs("  "); logs(nm); logs(": no element at +0xc\r\n"); } return; }
    if (oe_isVis(p, 0)) oe_hide(p);
}
/* ---- extra glass layers: the panel texture is only ~28% opaque, so stack copies of it ---- */
#define GLASS_LAYERS 4
typedef void* (__cdecl *SingletonFn)(void);
typedef void* (TC *CreateElemFn)(void*, const MsStr*, const MsStr*, char);
typedef char (TC *HasElemFn)(void*, const MsStr*, char);
typedef void* (TC *GetElemFn)(void*, const MsStr*, char);
typedef void (TC *AddChildFn)(void*, void*);
typedef void (TC *SetStrFn)(void*, const MsStr*);
typedef void (TC *Set4Fn)(void*, float, float, float, float);
typedef void (TC *Get4Fn)(void*, float*, float*, float*, float*);
typedef void (TC *Set2Fn)(void*, float, float);
typedef void (TC *Set1Fn)(void*, float);
static SingletonFn om_get; static CreateElemFn om_create; static HasElemFn om_has; static GetElemFn om_elem;
static AddChildFn oc_addChild; static SetStrFn pn_setMat; static Set4Fn pn_setUV; static Get4Fn pn_getUV;
static Set2Fn oe_setPos, oe_setDim; static Set1Fn oe_setAlpha; static int g_glassOk = -1, g_glassGen;
static void ms_short(MsStr* m, const char* c) { u32 n = 0; memset(m, 0, sizeof *m); while (c[n] && n < 15) { m->u.buf[n] = c[n]; n++; } m->u.buf[n] = 0; m->size = n; m->res = 15; }
static void glass_name(char* b, int which, int i) {   /* "HTglassA0_0": sorts before "Text..." so it draws underneath */
    const char* t = "HTglass"; int k = 0; while (t[k]) { b[k] = t[k]; k++; }
    b[k++] = (char)('A' + which); b[k++] = (char)('0' + i); b[k++] = '_'; b[k++] = (char)('0' + g_glassGen % 10); b[k] = 0; }
static void* glass_get(void* om, int which, int i) { char nm[16]; MsStr m; glass_name(nm, which, i); ms_short(&m, nm);
    return om_has(om, &m, 0) ? om_elem(om, &m, 0) : 0; }
static void glass_init(void) {
    om_get = (SingletonFn)ovl("?getSingletonPtr@OverlayManager@Ogre@@SAPAV12@XZ");
    om_create = (CreateElemFn)ovl("?createOverlayElement@OverlayManager@Ogre@@QAEPAVOverlayElement@2@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@0_N@Z");
    om_has = (HasElemFn)ovl("?hasOverlayElement@OverlayManager@Ogre@@QAE_NABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z");
    om_elem = (GetElemFn)ovl("?getOverlayElement@OverlayManager@Ogre@@QAEPAVOverlayElement@2@ABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@_N@Z");
    oc_addChild = (AddChildFn)ovl("?addChild@OverlayContainer@Ogre@@UAEXPAVOverlayElement@2@@Z");
    pn_setMat = (SetStrFn)ovl("?setMaterialName@PanelOverlayElement@Ogre@@UAEXABV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@@Z");
    pn_setUV = (Set4Fn)ovl("?setUV@PanelOverlayElement@Ogre@@QAEXMMMM@Z"); pn_getUV = (Get4Fn)ovl("?getUV@PanelOverlayElement@Ogre@@QBEXAAM000@Z");
    oe_setPos = (Set2Fn)ovl("?setPosition@OverlayElement@Ogre@@QAEXMM@Z"); oe_setDim = (Set2Fn)ovl("?setDimensions@OverlayElement@Ogre@@QAEXMM@Z");
    oe_setAlpha = (Set1Fn)ovl("?setAlpha@OverlayElement@Ogre@@QAEXM@Z");
    g_glassOk = om_get && om_create && om_has && om_elem && oc_addChild && pn_setMat && pn_setUV && pn_getUV && oe_setPos && oe_setDim && oe_setAlpha && oe_parent;
    if (!g_glassOk) logs("console style: glass layer functions missing\r\n");
}
static void glass_sync(void* orig, int which, float r, float g, float b) {
    void* om; int i; float u1, v1, u2, v2, a;
    if (!orig || *(void**)orig != vt_panel) return;
    void* parent = oe_parent(orig); if (!parent) return;
    om = om_get(); if (!om) return;
    pn_getUV(orig, &u1, &v1, &u2, &v2); a = oe_getAlpha(orig, 0);
    for (i = 0; i < GLASS_LAYERS; i++) {
        void* e = glass_get(om, which, i);
        if (e && oe_parent(e) != parent) {                 /* console was rebuilt */
            if (!oe_parent(e)) { oc_addChild(parent, e); logs("console style: glass layer moved to the new console\r\n"); }
            else { g_glassGen++; e = glass_get(om, which, i); } }
        if (!e) { char nm[16]; MsStr mt, mn; ms_short(&mt, "Panel"); glass_name(nm, which, i); ms_short(&mn, nm);
            e = om_create(om, &mt, &mn, 0); if (!e) return;
            { const MsStr* mat = oe_getMat(orig); if (mat) pn_setMat(e, mat); }
            oc_addChild(parent, e);
            if (i == 0) { logs("console style: glass layers for "); logs(which ? "input line\r\n" : "text area\r\n"); } }
        if (oe_left(e) != oe_left(orig) || oe_top(e) != oe_top(orig)) oe_setPos(e, oe_left(orig), oe_top(orig));
        if (oe_width(e) != oe_width(orig) || oe_height(e) != oe_height(orig)) oe_setDim(e, oe_width(orig), oe_height(orig));
        { float a1, b1, a2, b2; pn_getUV(e, &a1, &b1, &a2, &b2); if (a1 != u1 || b1 != v1 || a2 != u2 || b2 != v2) pn_setUV(e, u1, v1, u2, v2); }
        if (oe_getAlpha(e, 0) != a) oe_setAlpha(e, a);
        tint(e, r, g, b, 1.0f);
    }
}
static void con_style_frame(void) {
    u8* lay;
    if (!g_styleInit) { style_init(); pn_setColour = (OColSetFn)ovl("?setColour@PanelOverlayElement@Ogre@@UAEXABVColourValue@2@@Z");
        oe_hide = (OVoidFn)ovl("?hide@OverlayElement@Ogre@@UAEXXZ");
        if (g_styleInit && (!pn_setColour || !oe_hide || !vt_panel)) { g_styleInit = 0; } if (!g_styleInit) { logs("console style: OgreOverlay functions missing\r\n"); g_styleInit = -1; } }
    mod_descs_refresh();
    if (g_styleInit != 1 || !g_con) return;
    lay = *(u8**)(g_con + 8); if (!lay || IsBadReadPtr(lay, 0x190)) return;
    /* dark glass panels (the texture keeps its own soft edges and transparency) */
    { void* tb = map_find(lay + 0x9c, "TextBufferBack"), *il = map_find(lay + 0x9c, "TextInputLineBack");
      tint(tb, 0.05f, 0.06f, 0.08f, 1.0f); tint(il, 0.12f, 0.14f, 0.18f, 1.0f);
      if (g_glassOk < 0) glass_init();
      if (g_glassOk == 1) { glass_sync(tb, 0, 0.05f, 0.06f, 0.08f); glass_sync(il, 1, 0.12f, 0.14f, 0.18f); } }
    /* the mouse wheel scrolls now and B / Esc closes, so the old buttons go */
    if (!g_btnLogged) logs("console style: buttons\r\n");
    hide_button(map_find(lay + 0x1c, "TextInputScrollUp"), "ScrollUp");
    hide_button(map_find(lay + 0x1c, "TextInputScrollDown"), "ScrollDown");
    hide_button(map_find(lay + 0x1c, "TextInputCloseButton"), "Close");
    if (!g_btnLogged) { g_btnLogged = 1; logs("console style applied\r\n"); }
}

__declspec(dllexport) void dllStartPlugin(void) {
    g_log = CreateFileA("HobTools.log", 0x40000000, 1, 0, 2, 0x80, 0);
    logs("HobTools 0.9 beta loaded\r\n");
    load_keys();
    g_base = (u8*)GetModuleHandleA(0);
    HANDLE om = GetModuleHandleA("OgreMain.dll");
    QueryPerformanceFrequency(&g_qpf);
    if (!om) { logs("OgreMain not found\r\n"); return; }
    o_setPos = (CamVecFn)ogre(om, "?setPosition@Camera@Ogre@@QAEXABVVector3@2@@Z");
    o_setOri = (CamVecFn)ogre(om, "?setOrientation@Camera@Ogre@@QAEXABVQuaternion@2@@Z");
    o_lookAt = (CamVecFn)ogre(om, "?lookAt@Camera@Ogre@@QAEXABVVector3@2@@Z");
    o_yaw    = (CamVecFn)ogre(om, "?yaw@Camera@Ogre@@QAEXABVRadian@2@@Z");
    o_pitch  = (CamVecFn)ogre(om, "?pitch@Camera@Ogre@@QAEXABVRadian@2@@Z");
    o_getPos = (CamGetFn)ogre(om, "?getPosition@Camera@Ogre@@QBEABVVector3@2@XZ");
    o_getOri = (CamGetFn)ogre(om, "?getOrientation@Camera@Ogre@@QBEABVQuaternion@2@XZ");
    o_getDerOri = (CamGetFn)ogre(om, "?getDerivedOrientation@Camera@Ogre@@QBEABVQuaternion@2@XZ");
    o_getViewport = (PtrGetFn)ogre(om, "?getViewport@Camera@Ogre@@QBEPAVViewport@2@XZ");
    o_vpWidth = (IntGetFn)ogre(om, "?getActualWidth@Viewport@Ogre@@QBEHXZ");
    o_fireStarted = (RootFireFn)ogre(om, "?_fireFrameStarted@Root@Ogre@@QAE_NXZ");
    if (!o_setPos || !o_setOri || !o_lookAt || !o_getPos || !o_getOri || !o_getDerOri || !o_fireStarted) { logs("aborting\r\n"); return; }

    /* which build of Hob is this? */
    { int i; for (i = 0; i < (int)(sizeof g_builds / sizeof g_builds[0]); i++) if (build_matches(&g_builds[i])) { g_b = &g_builds[i]; break; } }
    if (!g_b) { logs("unknown Hob build - game features disabled (camera tools still work)\r\n"); }
    else {
    logs("game build: "); logs(g_b->name); logs("\r\n");
    g_gameOk = 1;
    g_getPlayer = (GetPlayerFn)(g_base + g_b->getPlayer); g_unitGetPos = (UnitGetPosFn)(g_base + g_b->unitGetPos);
    g_flagBD = (UnitBoolFn)(g_base + g_b->flagBD); g_inputClear = (InputClearFn)(g_base + g_b->inputClear);
    logs("game signatures OK (noclip available)\r\n");

    /* console text box (CTextInputBox) update -> lets us know the console is open */
    if (sig(g_b->setInput, "6aff68", 3)) {
        void** vt = (void**)(g_base + g_b->conVtable); u32 old;
        if (vt[25] == (void*)(g_base + g_b->conUpdate)) {
            o_conUpdate = (UpdFn)vt[25]; g_setInput = (SetInputFn)(g_base + g_b->setInput);
            VirtualProtect(&vt[25], 4, 0x04, &old); vt[25] = (void*)hk_conUpdate; VirtualProtect(&vt[25], 4, old, &old);
            g_conOk = 1; logs("console click-to-paste ready\r\n");
        }
    }
    /* Pause menu fix: when a sub-screen (Help/Options) closes, the game only slides the
       *hovered* button back to its home position (cmp [btn+0xc0],0 / je). If the mouse moved
       over another item on the way to "Back", the pressed button stays shifted right.
       NOP the je so every button is moved home (they're already there, so no visible change). */
    { u8* p = g_base + g_b->pauseCmp + 7; u32 old;
      if (VirtualProtect(p, 2, 0x40, &old)) { p[0] = 0x90; p[1] = 0x90; VirtualProtect(p, 2, old, &old); logs("pause menu fix applied\r\n"); } }
    if (sig(g_b->logRefresh, "6aff68", 3)) g_logRefresh = (RefreshFn)(g_base + g_b->logRefresh);
    console_safety();
    if (sig(g_b->logScroll, "8b4424048bd185c0", 8)) g_logScroll = (ScrollFn)(g_base + g_b->logScroll);
    }
    { HANDLE fm = GetModuleHandleA("fmodstudio.dll");
      if (fm) {
        o_fUpdate = (FUpd)GetProcAddress(fm, "?update@System@Studio@FMOD@@QAG?AW4FMOD_RESULT@@XZ");
        f_bankCount = (FGetCount)GetProcAddress(fm, "?getBankCount@System@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PAH@Z");
        f_bankList = (FGetList)GetProcAddress(fm, "?getBankList@System@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PAPAVBank@23@HPAH@Z");
        f_evCount = (FGetCount)GetProcAddress(fm, "?getEventCount@Bank@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PAH@Z");
        f_evList = (FGetList)GetProcAddress(fm, "?getEventList@Bank@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PAPAVEventDescription@23@HPAH@Z");
        f_getPath = (FGetPath)GetProcAddress(fm, "?getPath@EventDescription@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PADHPAH@Z");
        f_create = (FCreate)GetProcAddress(fm, "?createInstance@EventDescription@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PAPAVEventInstance@23@@Z");
        f_start = (FInst)GetProcAddress(fm, "?start@EventInstance@Studio@FMOD@@QAG?AW4FMOD_RESULT@@XZ");
        f_release = (FInst)GetProcAddress(fm, "?release@EventInstance@Studio@FMOD@@QAG?AW4FMOD_RESULT@@XZ");
        f_is3D = (FIs3D)GetProcAddress(fm, "?is3D@EventDescription@Studio@FMOD@@QBG?AW4FMOD_RESULT@@PA_N@Z");
        f_getLis = (FGetLis)GetProcAddress(fm, "?getListenerAttributes@System@Studio@FMOD@@QBG?AW4FMOD_RESULT@@HPAUFMOD_3D_ATTRIBUTES@@@Z");
        f_set3D = (FSet3D)GetProcAddress(fm, "?set3DAttributes@EventInstance@Studio@FMOD@@QAG?AW4FMOD_RESULT@@PBUFMOD_3D_ATTRIBUTES@@@Z");
        if (o_fUpdate) logx("hook fmod update: ", patch_iat((void*)o_fUpdate, (void*)hk_fUpdate));
      } else logs("fmodstudio.dll not loaded - using beeps\r\n"); }
    logx("hook frameStarted: ", patch_iat((void*)o_fireStarted, (void*)hk_fireStarted));
    logx("hook setPosition: ", patch_iat((void*)o_setPos, (void*)hk_setPos));
    logx("hook setOrientation: ", patch_iat((void*)o_setOri, (void*)hk_setOri));
    logx("hook lookAt: ", patch_iat((void*)o_lookAt, (void*)hk_lookAt));
    if (o_yaw) logx("hook yaw: ", patch_iat((void*)o_yaw, (void*)hk_yaw));
    if (o_pitch) logx("hook pitch: ", patch_iat((void*)o_pitch, (void*)hk_pitch));
}
__declspec(dllexport) void dllStopPlugin(void) { if (g_log != (HANDLE)-1) { CloseHandle(g_log); g_log = (HANDLE)-1; } }
