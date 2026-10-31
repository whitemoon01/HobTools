/* Hob Keybinds - native Win32, no CRT. Click a key, press the new key, save.
 * Right-click a key to restore its default. Edits PLAYER_CONTROLS.TXT in
 * Documents\My Games\Runic Games\HOB (and every SAVES\<id> copy).          */
typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32; typedef int BOOL;
typedef void* HANDLE; typedef HANDLE HWND, HDC, HFONT, HBRUSH, HPEN, HGDIOBJ, HINSTANCE, HBITMAP, HCURSOR;
typedef u16 WCHAR; typedef long LRESULT; typedef u32 WPARAM; typedef long LPARAM;
#define WINAPI __stdcall
#define IMP __declspec(dllimport)
#define L(s) ((const WCHAR*)L##s)
#include "i18n_kb.h"
typedef struct { long l, t, r, b; } RECT; typedef struct { long x, y; } POINT;
typedef LRESULT (WINAPI *WNDPROC)(HWND, u32, WPARAM, LPARAM);
typedef struct { u32 cbSize, style; WNDPROC proc; int cls, wnd; HINSTANCE inst; HANDLE icon; HCURSOR cur; HBRUSH bg; const WCHAR* menu; const WCHAR* name; HANDLE iconSm; } WNDCLASSEXW;
typedef struct { HWND hwnd; u32 msg; WPARAM wp; LPARAM lp; u32 time; POINT pt; } MSG;
typedef struct { HDC hdc; BOOL erase; RECT rc; BOOL restore, inc; u8 res[32]; } PAINTSTRUCT;
typedef struct { u32 cb, flags; HWND hwnd; u32 hover; } TRACKMOUSEEVENT;
typedef struct { u32 attr; u32 ft[6]; u32 sizeHi, sizeLo, r0, r1; WCHAR name[260]; WCHAR alt[14]; } WIN32_FIND_DATAW;

IMP HANDLE WINAPI GetModuleHandleW(const WCHAR*); IMP void WINAPI ExitProcess(u32);
IMP HANDLE WINAPI CreateFileW(const WCHAR*, u32, u32, void*, u32, u32, HANDLE);
IMP BOOL WINAPI ReadFile(HANDLE, void*, u32, u32*, void*); IMP BOOL WINAPI WriteFile(HANDLE, const void*, u32, u32*, void*);
IMP u32 WINAPI GetFileSize(HANDLE, u32*); IMP BOOL WINAPI CloseHandle(HANDLE);
IMP HANDLE WINAPI GetProcessHeap(void); IMP void* WINAPI HeapAlloc(HANDLE, u32, u32); IMP BOOL WINAPI HeapFree(HANDLE, u32, void*);
IMP HWND WINAPI FindWindowW(const WCHAR*, const WCHAR*);
IMP HANDLE WINAPI FindFirstFileW(const WCHAR*, WIN32_FIND_DATAW*); IMP BOOL WINAPI FindNextFileW(HANDLE, WIN32_FIND_DATAW*); IMP BOOL WINAPI FindClose(HANDLE);
IMP BOOL WINAPI CopyFileW(const WCHAR*, const WCHAR*, BOOL); IMP u32 WINAPI GetFileAttributesW(const WCHAR*); IMP u32 WINAPI GetTickCount(void);
IMP long WINAPI SHGetFolderPathW(HWND, int, HANDLE, u32, WCHAR*);
IMP u16 WINAPI RegisterClassExW(const WNDCLASSEXW*);
IMP HWND WINAPI CreateWindowExW(u32, const WCHAR*, const WCHAR*, u32, int, int, int, int, HWND, HANDLE, HINSTANCE, void*);
IMP BOOL WINAPI ShowWindow(HWND, int); IMP BOOL WINAPI UpdateWindow(HWND); IMP BOOL WINAPI DestroyWindow(HWND);
IMP BOOL WINAPI GetMessageW(MSG*, HWND, u32, u32); IMP BOOL WINAPI TranslateMessage(const MSG*); IMP LRESULT WINAPI DispatchMessageW(const MSG*);
IMP LRESULT WINAPI DefWindowProcW(HWND, u32, WPARAM, LPARAM); IMP void WINAPI PostQuitMessage(int);
IMP HCURSOR WINAPI LoadCursorW(HINSTANCE, const WCHAR*); IMP HCURSOR WINAPI SetCursor(HCURSOR);
IMP BOOL WINAPI InvalidateRect(HWND, const RECT*, BOOL); IMP HDC WINAPI BeginPaint(HWND, PAINTSTRUCT*); IMP BOOL WINAPI EndPaint(HWND, const PAINTSTRUCT*);
IMP BOOL WINAPI GetClientRect(HWND, RECT*); IMP u32 WINAPI SetTimer(HWND, u32, u32, void*); IMP int WINAPI FillRect(HDC, const RECT*, HBRUSH);
IMP int WINAPI DrawTextW(HDC, const WCHAR*, int, RECT*, u32); IMP BOOL WINAPI TrackMouseEvent(TRACKMOUSEEVENT*);
IMP BOOL WINAPI AdjustWindowRectEx(RECT*, u32, BOOL, u32); IMP int WINAPI GetSystemMetrics(int); IMP BOOL WINAPI SetProcessDPIAware(void);
IMP u32 WINAPI MapVirtualKeyW(u32, u32); IMP HDC WINAPI GetDC(HWND); IMP int WINAPI ReleaseDC(HWND, HDC);
IMP int WINAPI MessageBoxW(HWND, const WCHAR*, const WCHAR*, u32);
IMP HFONT WINAPI CreateFontW(int, int, int, int, int, u32, u32, u32, u32, u32, u32, u32, u32, const WCHAR*);
IMP HGDIOBJ WINAPI SelectObject(HDC, HGDIOBJ); IMP BOOL WINAPI DeleteObject(HGDIOBJ); IMP int WINAPI SetBkMode(HDC, int);
IMP u32 WINAPI SetTextColor(HDC, u32); IMP HBRUSH WINAPI CreateSolidBrush(u32); IMP HPEN WINAPI CreatePen(int, int, u32);
IMP BOOL WINAPI RoundRect(HDC, int, int, int, int, int, int); IMP HDC WINAPI CreateCompatibleDC(HDC);
IMP HBITMAP WINAPI CreateCompatibleBitmap(HDC, int, int); IMP BOOL WINAPI BitBlt(HDC, int, int, int, int, HDC, int, int, u32);
IMP BOOL WINAPI DeleteDC(HDC); IMP int WINAPI GetDeviceCaps(HDC, int); IMP HGDIOBJ WINAPI GetStockObject(int);
IMP int WINAPI IntersectClipRect(HDC, int, int, int, int); IMP int WINAPI SelectClipRgn(HDC, HANDLE);
IMP long WINAPI DwmSetWindowAttribute(HWND, u32, const void*, u32);
IMP BOOL WINAPI AlphaBlend(HDC, int, int, int, int, HDC, int, int, int, int, u32);
typedef struct { u32 size; long w, h; u16 planes, bpp; u32 comp, imgSize; long xppm, yppm; u32 used, important; u32 colors[1]; } BMI;
IMP HBITMAP WINAPI CreateDIBSection(HDC, const BMI*, u32, void**, HANDLE, u32);
IMP BOOL WINAPI Ellipse(HDC, int, int, int, int); IMP HWND WINAPI SetCapture(HWND); IMP BOOL WINAPI ReleaseCapture(void);
IMP HANDLE WINAPI LoadIconW(HINSTANCE, const WCHAR*);
typedef struct { long cx, cy; } SIZE;
IMP BOOL WINAPI GetTextExtentPoint32W(HDC, const WCHAR*, int, SIZE*); IMP WCHAR* WINAPI CharUpperW(WCHAR*);

int _fltused = 0;
void* memset(void* d, int c, u32 n) { u8* p = d; while (n--) *p++ = (u8)c; return d; }
void* memcpy(void* d, const void* s, u32 n) { u8* a = d; const u8* b = s; while (n--) *a++ = *b++; return d; }
static void* alloc(u32 n) { return HeapAlloc(GetProcessHeap(), 8, n); }
static int slen(const char* s) { int n = 0; while (s[n]) n++; return n; }
static int seq(const char* a, const char* b) { while (*a && *a == *b) a++, b++; return *a == *b; }
static int seq8(const char* a, const char* pre) { while (*pre && *a == *pre) a++, pre++; return !*pre; }
static int wlen(const WCHAR* s) { int n = 0; while (s[n]) n++; return n; }
static void wcat(WCHAR* d, const WCHAR* s) { d += wlen(d); while ((*d++ = *s++)); }
static void wcpy(WCHAR* d, const WCHAR* s) { while ((*d++ = *s++)); }
static void wcatA(WCHAR* d, const char* s) { d += wlen(d); while ((*d++ = (u8)*s++)); }
/* ------------------------------------------------------------ language */
static int g_lg; static int g_rtl;
static const WCHAR* g_tr[K__N];
static const WCHAR* TR(int id) { return g_tr[id] ? g_tr[id] : g_kbEn[id]; }
/* copy string id into out, replacing {1}..{4} */
static void tf(WCHAR* out, int cap, int id, const WCHAR* a1, const WCHAR* a2, const WCHAR* a3, const WCHAR* a4) {
    const WCHAR* t = TR(id); const WCHAR* av[4] = { a1, a2, a3, a4 }; int n = 0;
    while (*t && n < cap - 1) {
        if (t[0] == '{' && t[1] >= '1' && t[1] <= '4' && t[2] == '}') { const WCHAR* q = av[t[1] - '1']; if (q) while (*q && n < cap - 1) out[n++] = *q++; t += 3; }
        else out[n++] = *t++; }
    out[n] = 0; }
static int has_kw(const char* s, const char* w) { int i, k; for (i = 0; s[i]; i++) { for (k = 0; w[k] && s[i + k] && (s[i + k] & ~32) == (w[k] & ~32); k++); if (!w[k]) return 1; } return 0; }
static int lang_from_name(const char* s) {     /* Hob's LANGUAGE setting, e.g. FRENCH */
    if (has_kw(s, "BRAZ") || (has_kw(s, "PORT") && has_kw(s, "BR"))) return LG_PT_BR;
    if (has_kw(s, "PORT")) return LG_PT;
    if (has_kw(s, "TRAD") || has_kw(s, "TCHI") || has_kw(s, "TAIW") || has_kw(s, "ZH_TW") || has_kw(s, "HANT")) return LG_ZH_TW;
    if (has_kw(s, "CHIN") || has_kw(s, "SIMP") || has_kw(s, "ZH")) return LG_ZH_CN;
    if (has_kw(s, "FRENCH") || has_kw(s, "FRAN")) return LG_FR;
    if (has_kw(s, "GERMAN") || has_kw(s, "DEUT")) return LG_DE;
    if (has_kw(s, "SPAN") || has_kw(s, "ESPA")) return LG_ES;
    if (has_kw(s, "ITAL")) return LG_IT;
    if (has_kw(s, "DANISH") || has_kw(s, "DANSK")) return LG_DA;
    if (has_kw(s, "DUTCH") || has_kw(s, "NEDER") || has_kw(s, "NETHER")) return LG_NL;
    if (has_kw(s, "FINN") || has_kw(s, "SUOMI")) return LG_FI;
    if (has_kw(s, "NORW") || has_kw(s, "NORSK")) return LG_NO;
    if (has_kw(s, "SWED") || has_kw(s, "SVENSK")) return LG_SV;
    if (has_kw(s, "POLI") || has_kw(s, "POLSK")) return LG_PL;
    if (has_kw(s, "TURK")) return LG_TR;
    if (has_kw(s, "RUSS")) return LG_RU;
    if (has_kw(s, "JAPA")) return LG_JA;
    if (has_kw(s, "KORE")) return LG_KO;
    if (has_kw(s, "ARAB")) return LG_AR;
    return LG_EN;
}
static int wieq(const WCHAR* a, const WCHAR* b) {
    for (;; a++, b++) { WCHAR x = *a, y = *b; if (x >= 'a' && x <= 'z') x -= 32; if (y >= 'a' && y <= 'z') y -= 32; if (x != y) return 0; if (!x) return 1; } }

/* ------------------------------------------------------------------ data */
typedef struct { int secId, labelId; const char* action; int slot; const char* also[6]; const char* ctx; const char* def;
                 int descId; const WCHAR *sec, *label, *desc; char cur[24]; int y; RECT chip, rowrc; } Group;
#define D(x) L(x)
static Group groups[] = {
    { K_SEC_MOVE, K_L_UP, "MOVEMENT_UP", 0, {0}, "game", "W", K_D_UP },
    { K_SEC_MOVE, K_L_DOWN, "MOVEMENT_DOWN", 0, {0}, "game", "S", K_D_DOWN },
    { K_SEC_MOVE, K_L_LEFT, "MOVEMENT_LEFT", 0, {0}, "game", "A", K_D_LEFT },
    { K_SEC_MOVE, K_L_RIGHT, "MOVEMENT_RIGHT", 0, {0}, "game", "D", K_D_RIGHT },
    { K_SEC_MOVE, K_L_WALK, "MOVEMENT_FORCEWALK", 0, {0}, "game", "V", K_D_WALK },
    { K_SEC_MOVE, K_L_SPRINT, "SPRINT", 1, {"LEDGE_CLIMB", "WALL_CLIMB"}, "game", "SHIFT", K_D_SPRINT },
    { K_SEC_MOVE, K_L_JUMP, "JUMP", 1, {"WALL_JUMP"}, "game", "SPACE", K_D_JUMP },
    { K_SEC_MOVE, K_L_ROLL, "ROLL", 5, {0}, "game", "C", K_D_ROLL },
    { K_SEC_MOVE, K_L_MOUSEMOVE, "MOUSE_MOVE", 0, {0}, "game", "MOUSE_RIGHT", K_D_MOUSEMOVE },
    { K_SEC_COMBAT, K_L_ATTACK, "LIGHT_ATTACK", 1, {0}, "game", "MOUSE_LEFT", K_D_ATTACK },
    { K_SEC_COMBAT, K_L_PUNCH, "GLOVE_PUNCH", 1, {"HEAVY_ATTACK", "HEAVY_CHARGE"}, "game", "R", K_D_PUNCH },
    { K_SEC_COMBAT, K_L_SHIELD, "BLOCK", 1, {0}, "game", "CTRL", K_D_SHIELD },
    { K_SEC_COMBAT, K_L_LOCKON, "LOCKON_TOGGLE", 2, {0}, "game", "TAB", K_D_LOCKON },
    { K_SEC_COMBAT, K_L_GRAPPLE, "GLOVE_MAGNET", 1, {0}, "game", "Q", K_D_GRAPPLE },
    { K_SEC_COMBAT, K_L_WARP, "GLOVE_WARP", 1, {0}, "game", "F", K_D_WARP },
    { K_SEC_WORLD, K_L_INTERACT, "INTERACT", 2, {"PUSH_PULL", "DROP", "SHEATHE_SWORD", "WALL_DISMOUNT", "CANCEL"}, "game", "E", K_D_INTERACT },
    { K_SEC_WORLD, K_L_SKIP, "SKIP_SCRIPTS", 1, {0}, "cutscene", "SPACE", K_D_SKIP },
    { K_SEC_MENUS, K_L_MAP, "WORLD_MENU", 0, {0}, "game", "M", K_D_MAP },
    { K_SEC_MENUS, K_L_INV, "INVENTORY_MENU", 1, {0}, "game", "I", K_D_INV },
    { K_SEC_MENUS, K_L_UPG, "UPGRADE_MENU", 0, {0}, "game", "U", K_D_UPG },
    { K_SEC_MENUS, K_L_PAUSE, "PAUSE", 1, {0}, "game", "ESC", K_D_PAUSE },
    { K_SEC_MENUS, K_L_CONFIRM, "SELECT", 1, {0}, "menu", "ENTER", K_D_CONFIRM },
    { K_SEC_MENUS, K_L_BACK, "CANCEL", 2, {0}, "menu", "ESC", K_D_BACK },
    { K_SEC_MENUS, K_L_ZOOMIN, "UI_ZOOM_IN", 1, {0}, "menu", "PLUS", K_D_ZOOMIN },
    { K_SEC_MENUS, K_L_ZOOMOUT, "UI_ZOOM_OUT", 1, {0}, "menu", "MINUS", K_D_ZOOMOUT },
    { K_SEC_MOD, K_L_CONSOLE, "CONSOLE", 0, {0}, "game", "B", K_D_CONSOLE },
    { K_SEC_MOD, K_L_NOCLIP, "@NOCLIP", 0, {0}, "game", "F6", K_D_NOCLIP },
    { K_SEC_MOD, K_L_FREECAM, "@FREECAM", 0, {0}, "game", "F7", K_D_FREECAM },
    { K_SEC_MOD, K_L_TELEPORT, "@TELEPORT", 0, {0}, "game", "F8", K_D_TELEPORT },
    { K_SEC_MOD, K_L_FOLLOW, "@FOLLOW", 0, {0}, "game", "F9", K_D_FOLLOW },
    { K_SEC_MOD, K_L_FASTER, "@SPEEDUP", 0, {0}, "game", "PAGEUP", K_D_FASTER },
    { K_SEC_MOD, K_L_SLOWER, "@SPEEDDOWN", 0, {0}, "game", "PAGEDOWN", K_D_SLOWER },
};
#define NG ((int)(sizeof groups / sizeof groups[0]))

/* --------------------------------------------------------- controls files */
typedef struct { WCHAR path[600]; char** line; int n, cap; } CFile;
static CFile files[8]; static int nfiles;

static int load_file(CFile* f) {
    HANDLE h = CreateFileW(f->path, 0x80000000, 3, 0, 3, 0x80, 0); if (h == (HANDLE)-1) return 0;
    u32 sz = GetFileSize(h, 0), rd; char* b = alloc(sz + 2); ReadFile(h, b, sz, &rd, 0); CloseHandle(h); b[rd] = 0;
    f->cap = 1024; f->line = alloc(f->cap * 4); f->n = 0;
    char* p = b;
    for (;;) {
        char* s = p; while (*p && *p != '\n') p++;
        int len = (int)(p - s); if (len && s[len - 1] == '\r') len--;
        char* l = alloc(len + 40); memcpy(l, s, len); l[len] = 0;
        if (f->n < f->cap) f->line[f->n++] = l;
        if (!*p) break; p++;
    }
    return 1;
}
static int save_file(CFile* f) {
    HANDLE h = CreateFileW(f->path, 0x40000000, 0, 0, 2, 0x80, 0); if (h == (HANDLE)-1) return 0;
    int i; u32 w;
    for (i = 0; i < f->n; i++) { WriteFile(h, f->line[i], slen(f->line[i]), &w, 0); if (i + 1 < f->n) WriteFile(h, "\r\n", 2, &w, 0); }
    CloseHandle(h); return 1;
}
static const char* trim(const char* s) { while (*s == ' ' || *s == '\t') s++; return s; }
static int is_tag(const char* t, const char* a, int close) { /* "[A]" or "[/A]" */
    if (*t++ != '[') return 0; if (close) { if (*t++ != '/') return 0; }
    while (*a && *t == *a) t++, a++; return !*a && t[0] == ']' && !t[1]; }
static int input_prefix(const char* t, int slot) { /* "<STRING>INPUTn:" -> length or 0 */
    const char* p = "<STRING>INPUT"; int i = 0; while (p[i]) { if (t[i] != p[i]) return 0; i++; }
    if (slot < 0) { while (t[i] >= '0' && t[i] <= '9') i++; }
    else { if (t[i] != '0' + slot) return 0; i++; }
    return t[i] == ':' ? i + 1 : 0; }
static int find_line(CFile* f, const char* action, int slot) {
    int i, in = 0;
    for (i = 0; i < f->n; i++) { const char* t = trim(f->line[i]);
        if (is_tag(t, action, 0)) { in = 1; continue; } if (is_tag(t, action, 1)) return -1;
        if (in && input_prefix(t, slot)) return i; }
    return -1;
}
static void set_value(char* line, const char* v) { char* c = line + slen(line); while (c > line && c[-1] != ':') c--; int n = slen(v); memcpy(c, v, n + 1); }
static void replace_values(CFile* f, const char* action, const char* oldv, const char* newv) {
    int i, in = 0;
    for (i = 0; i < f->n; i++) { const char* t = trim(f->line[i]);
        if (is_tag(t, action, 0)) { in = 1; continue; } if (is_tag(t, action, 1)) return;
        int k; if (in && (k = input_prefix(t, -1)) && seq(t + k, oldv)) set_value(f->line[i], newv); }
}

static WCHAR hobDir[400]; static int g_dirty;
static WCHAR iniPath[600]; static char g_langName[32];
IMP u32 WINAPI GetModuleFileNameW(HANDLE, WCHAR*, u32);
static int is_mod(Group* g) { return g->action[0] == '@'; }
static void load_ini(void) {
    CFile f; int i, j; wcpy(f.path, iniPath);
    for (i = 0; i < NG; i++) if (is_mod(&groups[i])) { int n = slen(groups[i].def); memcpy(groups[i].cur, groups[i].def, n + 1); }
    if (!load_file(&f)) return;
    for (j = 0; j < f.n; j++) { const char* t = trim(f.line[j]);
        for (i = 0; i < NG; i++) if (is_mod(&groups[i])) { const char* a = groups[i].action + 1; int k = 0;
            while (a[k] && t[k] == a[k]) k++;
            if (!a[k] && t[k] == '=') { const char* v = t + k + 1; int n = slen(v); while (n && (v[n-1] == ' ' || v[n-1] == '\r')) n--;
                if (n > 0 && n < 24) { memcpy(groups[i].cur, v, n); groups[i].cur[n] = 0; } } } }
}
static int save_ini(void) {
    HANDLE h = CreateFileW(iniPath, 0x40000000, 0, 0, 2, 0x80, 0); if (h == (HANDLE)-1) return 0;
    u32 w; int i; const char* hd = "; HobTools mod keys - change them with Hob Keybinds.exe\r\n[Keys]\r\n"; WriteFile(h, hd, slen(hd), &w, 0);
    for (i = 0; i < NG; i++) if (is_mod(&groups[i])) { WriteFile(h, groups[i].action + 1, slen(groups[i].action + 1), &w, 0); WriteFile(h, "=", 1, &w, 0);
        WriteFile(h, groups[i].cur, slen(groups[i].cur), &w, 0); WriteFile(h, "\r\n", 2, &w, 0); }
    if (g_langName[0]) { WriteFile(h, "LANGUAGE=", 9, &w, 0); WriteFile(h, g_langName, slen(g_langName), &w, 0); WriteFile(h, "\r\n", 2, &w, 0); }
    CloseHandle(h); return 1;
}
static void load_all(void) {
    WCHAR pat[600]; WIN32_FIND_DATAW fd; int i; nfiles = 0;
    wcpy(pat, hobDir); wcat(pat, L("\\SAVES\\*"));
    HANDLE h = FindFirstFileW(pat, &fd);
    if (h != (HANDLE)-1) { do { if ((fd.attr & 0x10) && fd.name[0] != '.' && nfiles < 7) {
        CFile* f = &files[nfiles]; wcpy(f->path, hobDir); wcat(f->path, L("\\SAVES\\")); wcat(f->path, fd.name); wcat(f->path, L("\\PLAYER_CONTROLS.TXT"));
        if (GetFileAttributesW(f->path) != 0xFFFFFFFF && load_file(f)) nfiles++; } } while (FindNextFileW(h, &fd)); FindClose(h); }
    CFile* r = &files[nfiles]; wcpy(r->path, hobDir); wcat(r->path, L("\\PLAYER_CONTROLS.TXT"));
    if (load_file(r)) nfiles++;
    for (i = 0; i < nfiles; i++) { WCHAR o[620]; wcpy(o, files[i].path); wcat(o, L(".original")); CopyFileW(files[i].path, o, 1); }
    for (i = 0; i < NG; i++) { groups[i].cur[0] = 0; if (is_mod(&groups[i])) continue;
        if (nfiles) { int li = find_line(&files[0], groups[i].action, groups[i].slot);
            if (li >= 0) { const char* t = trim(files[0].line[li]); int k = input_prefix(t, groups[i].slot); int n = slen(t + k); if (n > 23) n = 23; memcpy(groups[i].cur, t + k, n); groups[i].cur[n] = 0; } } }
}
static void apply(Group* g, const char* tok) {
    char old[24]; int i, j; memcpy(old, g->cur, 24);
    if (!is_mod(g)) for (i = 0; i < nfiles; i++) {
        int li = find_line(&files[i], g->action, g->slot); if (li >= 0) set_value(files[i].line[li], tok);
        if (old[0]) for (j = 0; j < 6 && g->also[j]; j++) replace_values(&files[i], g->also[j], old, tok);
    }
    i = slen(tok); if (i > 23) i = 23; memcpy(g->cur, tok, i); g->cur[i] = 0; g_dirty = 1;
}
static int hob_running(void) {
    return FindWindowW(L("OgreD3D11Wnd"), 0) != 0;   /* Hob's game window */
}

/* ------------------------------------------------------------ key names */
static const char* token_for_vk(u32 vk) {
    static char one[2];
    static char fk[4];
    if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9')) { one[0] = (char)vk; one[1] = 0; return one; }
    if (vk >= 0x70 && vk <= 0x7B) { int n = vk - 0x6F; fk[0] = 'F'; if (n >= 10) { fk[1] = '1'; fk[2] = (char)('0' + n - 10); fk[3] = 0; } else { fk[1] = (char)('0' + n); fk[2] = 0; } return fk; }
    switch (vk) {
    case 0x10: case 0xA0: case 0xA1: return "SHIFT"; case 0x11: case 0xA2: case 0xA3: return "CTRL"; case 0x12: case 0xA4: case 0xA5: return "ALT";
    case 0x20: return "SPACE"; case 0x09: return "TAB"; case 0x0D: return "ENTER"; case 0x26: return "UP"; case 0x28: return "DOWN";
    case 0x25: return "LEFT"; case 0x27: return "RIGHT"; case 0xC0: return "TILDA"; case 0x23: return "END"; case 0x2E: return "DEL";
    case 0x08: return "BACKSPACE"; case 0x14: return "CAPSLOCK"; case 0x21: return "PAGEUP"; case 0x22: return "PAGEDOWN"; case 0x24: return "HOME";
    case 0x2D: return "INSERT"; case 0xBA: return "SEMICOLON"; case 0xBF: return "FORWARDSLASH"; case 0xDB: return "LEFTBRACKET";
    case 0xDC: return "BACKSLASH"; case 0xDD: return "RIGHTBRACKET"; case 0xDE: return "QUOTE"; case 0xBC: return "COMMA";
    case 0xBE: return "PERIOD"; case 0xBD: return "MINUS"; case 0xBB: return "PLUS"; }
    return 0;
}
static const struct { const char* t; const char* n; u32 vk; } names[] = {
    {"SHIFT","Shift"},{"CTRL","Ctrl"},{"ALT","Alt"},{"SPACE","Space"},{"TAB","Tab"},{"ENTER","Enter"},{"UP","Up arrow"},{"DOWN","Down arrow"},
    {"LEFT","Left arrow"},{"RIGHT","Right arrow"},{"END","End"},{"ESC","Esc"},{"DEL","Delete"},{"BACKSPACE","Backspace"},{"CAPSLOCK","Caps Lock"},
    {"PAGEUP","Page Up"},{"PAGEDOWN","Page Down"},{"HOME","Home"},{"INSERT","Insert"},{"MOUSE_LEFT","Left mouse"},{"MOUSE_RIGHT","Right mouse"},
    {"MOUSE_MIDDLE","Middle mouse"},{"MOUSE_BUTTON1","Mouse side 1"},{"MOUSE_BUTTON2","Mouse side 2"},{"MOUSE_WHEEL_UP","Wheel up"},
    {"MOUSE_WHEEL_DOWN","Wheel down"},{"MOUSE_LEFT_RIGHT","Both mouse"},
    {"TILDA",0,0xC0},{"SEMICOLON",0,0xBA},{"FORWARDSLASH",0,0xBF},{"LEFTBRACKET",0,0xDB},{"BACKSLASH",0,0xDC},{"RIGHTBRACKET",0,0xDD},
    {"QUOTE",0,0xDE},{"COMMA",0,0xBC},{"PERIOD",0,0xBE},{"MINUS",0,0xBD},{"PLUS",0,0xBB} };
static int key_name_id(const char* t) {
    static const struct { const char* t; int id; } m[] = { {"SPACE",K_K_SPACE},{"UP",K_K_UP},{"DOWN",K_K_DOWN},{"LEFT",K_K_LEFT},{"RIGHT",K_K_RIGHT},
        {"MOUSE_LEFT",K_K_LMB},{"MOUSE_RIGHT",K_K_RMB},{"MOUSE_MIDDLE",K_K_MMB},{"MOUSE_BUTTON1",K_K_MB1},{"MOUSE_BUTTON2",K_K_MB2},
        {"MOUSE_WHEEL_UP",K_K_WUP},{"MOUSE_WHEEL_DOWN",K_K_WDOWN},{"MOUSE_LEFT_RIGHT",K_K_BOTH},{"DEL",K_K_DEL} };
    int i; for (i = 0; i < (int)(sizeof m / sizeof m[0]); i++) if (seq(m[i].t, t)) return m[i].id; return -1; }
static void pretty(const char* tok, WCHAR* out) {
    int i; out[0] = 0;
    if (!tok || !tok[0]) { wcpy(out, L("-")); return; }
    if ((i = key_name_id(tok)) >= 0) { wcpy(out, TR(i)); return; }
    for (i = 0; i < (int)(sizeof names / sizeof names[0]); i++) if (seq(names[i].t, tok)) {
        if (names[i].n) { wcatA(out, names[i].n); return; }
        u32 ch = MapVirtualKeyW(names[i].vk, 2) & 0x7FFF;
        if (ch > 32) { if (ch >= 'a' && ch <= 'z') ch -= 32; if (ch >= 0xE0 && ch <= 0xFE && ch != 0xF7) ch -= 32; out[0] = (WCHAR)ch; out[1] = 0; return; }
        break; }
    wcatA(out, tok);
}

/* ----------------------------------------------------------------- UI */
#define RGB(r,g,b) ((u32)(r) | ((u32)(g) << 8) | ((u32)(b) << 16))
static const u32 C_BG = RGB(30,26,33), C_ROWH = RGB(42,37,47), C_TEXT = RGB(222,240,233),
    C_MUTED = RGB(124,128,136), C_TEAL = RGB(70,205,190), C_CHIP = RGB(36,32,40), C_CHIPB = RGB(120,180,168),
    C_WARN = RGB(240,140,70), C_DARK = RGB(22,18,24), C_JADE = RGB(168,214,196), C_ORANGE = RGB(240,138,60), C_LINE = RGB(52,46,58);
static HWND g_wnd; static float S = 1; static HFONT fTitle, fSub, fRow, fChip, fSec, fBtn;
static int g_scroll, g_contentH, g_hoverRow = -1, g_hoverBtn = -1, g_capture = -1, g_blink;
static int g_drag, g_dragY0, g_dragS0; static RECT g_thumb;
static WCHAR g_status[400]; static u32 g_statusCol; static u32 g_ignoreUntil;
static int sc(int v) { return (int)(v * S + 0.5f); }
enum { B_SAVE, B_SAVECLOSE, B_RESET, B_CLOSE, NB };
static const WCHAR* btnText[NB]; static RECT btnRc[NB];
/* modal */
static int m_open, m_kind, m_group, m_n; static char m_tok[24]; static WCHAR m_title[200], m_body[600]; static const WCHAR* m_btn[3]; static RECT m_rc[3]; static int m_hover = -1;
static int m_clash[NG];
enum { MK_CLASH, MK_RESET, MK_UNSAVED };

static u32 g_statusAt;
static void status(const WCHAR* s, u32 col) { wcpy(g_status, s); g_statusCol = col; g_statusAt = GetTickCount(); InvalidateRect(g_wnd, 0, 0); }
static void list_area(RECT* r) { RECT c; GetClientRect(g_wnd, &c); r->l = 0; r->t = sc(92); r->r = c.r; r->b = c.b - sc(130); }

static void rr(HDC dc, RECT r, int rad, u32 fill, u32 border, int bw) {
    HBRUSH b = CreateSolidBrush(fill); HPEN p = bw ? CreatePen(0, bw, border) : (HPEN)GetStockObject(8);
    HGDIOBJ ob = SelectObject(dc, b), op = SelectObject(dc, p);
    RoundRect(dc, r.l, r.t, r.r, r.b, rad, rad);
    SelectObject(dc, ob); SelectObject(dc, op); DeleteObject(b); if (bw) DeleteObject(p);
}
static void txt(HDC dc, const WCHAR* s, RECT r, u32 col, HFONT f, u32 fl) {
    if (g_rtl) { fl |= 0x20000; if (!(fl & 1)) fl |= 2; }     /* Arabic: right-to-left, right aligned */
    SelectObject(dc, f); SetTextColor(dc, col); DrawTextW(dc, s, -1, &r, fl | 0x800); }
static int tw(HDC dc, HFONT f, const WCHAR* s) { SIZE z; z.cx = 0; SelectObject(dc, f); GetTextExtentPoint32W(dc, s, wlen(s), &z); return z.cx; }
static void fill(HDC dc, RECT r, u32 col) { HBRUSH b = CreateSolidBrush(col); FillRect(dc, &r, b); DeleteObject(b); }
static void dim(HDC dc, RECT r, u32 col, int alpha) {
    HDC m = CreateCompatibleDC(dc); HBITMAP bm = CreateCompatibleBitmap(dc, 1, 1); HGDIOBJ ob = SelectObject(m, bm);
    RECT one = { 0, 0, 1, 1 }; fill(m, one, col);
    AlphaBlend(dc, r.l, r.t, r.r - r.l, r.b - r.t, m, 0, 0, 1, 1, (u32)alpha << 16);
    SelectObject(m, ob); DeleteObject(bm); DeleteDC(m);
}
static void ring(HDC dc, int cx, int cy, int rad, int w, u32 col) {
    HPEN p = CreatePen(0, w, col); HGDIOBJ op = SelectObject(dc, p), ob = SelectObject(dc, GetStockObject(5));
    Ellipse(dc, cx - rad, cy - rad, cx + rad, cy + rad); SelectObject(dc, op); SelectObject(dc, ob); DeleteObject(p);
}
static void keycap(HDC dc, RECT k, u32 face, u32 edge, u32 border, int bw) {
    RECT base = k; base.t += sc(3); rr(dc, base, sc(9), edge, 0, 0);
    RECT top = k; top.b -= sc(3); rr(dc, top, sc(9), face, border, bw);
}
static int changed_count(void) { int i, n = 0; for (i = 0; i < NG; i++) if (groups[i].cur[0] && !seq(groups[i].cur, groups[i].def)) n++; return n; }

static void layout(void) {
    int i, y = sc(8); const WCHAR* sec = 0;
    for (i = 0; i < NG; i++) {
        if (!sec || !wieq(sec, groups[i].sec)) { sec = groups[i].sec; y += sc(i ? 22 : 4); y += sc(28); }
        groups[i].y = y; y += sc(40);
    }
    g_contentH = y + sc(10);
}
/* ---- anti-aliased logo: teal ring + orange dot, 4x4 supersampled into a 32-bit premultiplied bitmap ---- */
static HDC g_logoDC; static int g_logoSz;
static float sqrtf_(float x) { float r; __asm__("sqrtss %1, %0" : "=x"(r) : "x"(x)); return r; }
static void make_logo(HDC ref) {
    int sz = sc(40), x, y, sx, sy; BMI bi; void* bits = 0;
    memset(&bi, 0, sizeof bi); bi.size = 40; bi.w = sz; bi.h = -sz; bi.planes = 1; bi.bpp = 32;
    HBITMAP bm = CreateDIBSection(ref, &bi, 0, &bits, 0, 0); if (!bm || !bits) return;
    float c = sz / 2.0f, R = 15.0f * S, hw = 2.2f * S, rd = 4.2f * S;
    u32* px = (u32*)bits;
    for (y = 0; y < sz; y++) for (x = 0; x < sz; x++) {
        float ar = 0, ag = 0, ab = 0, aa = 0;
        for (sy = 0; sy < 4; sy++) for (sx = 0; sx < 4; sx++) {
            float dx = x + (sx + 0.5f) / 4 - c, dy = y + (sy + 0.5f) / 4 - c, d = sqrtf_(dx * dx + dy * dy);
            if (d <= rd) { ar += 240; ag += 138; ab += 60; aa += 1; }
            else if (d >= R - hw && d <= R + hw) { ar += 70; ag += 205; ab += 190; aa += 1; }
        }
        ar /= 16; ag /= 16; ab /= 16; aa /= 16;           /* colour sums are already premultiplied */
        px[y * sz + x] = ((u32)(aa * 255 + 0.5f) << 24) | ((u32)(ar + 0.5f) << 16) | ((u32)(ag + 0.5f) << 8) | (u32)(ab + 0.5f);
    }
    g_logoDC = CreateCompatibleDC(ref); SelectObject(g_logoDC, bm); g_logoSz = sz;
}
static void draw_logo(HDC dc, int cx, int cy) {
    if (!g_logoDC) make_logo(dc);
    if (g_logoDC) AlphaBlend(dc, cx - g_logoSz / 2, cy - g_logoSz / 2, g_logoSz, g_logoSz, g_logoDC, 0, 0, g_logoSz, g_logoSz, 0x01FF0000);
}
static void outline_key(HDC dc, RECT k, u32 fillc, u32 border, int bw) { rr(dc, k, sc(6), fillc, border, bw); }
static void draw(HDC dc) {
    RECT c, r, la; int i; GetClientRect(g_wnd, &c); list_area(&la);
    fill(dc, c, C_BG); SetBkMode(dc, 1);
    /* header */
    draw_logo(dc, sc(44), sc(46));
    r = c; r.l = sc(72); r.t = sc(24); r.b = sc(68); txt(dc, TR(K_TITLE), r, C_JADE, fTitle, 0x20 | 4);
    r = c; r.t = sc(84); r.b = sc(85); r.l = sc(28); r.r -= sc(28); fill(dc, r, C_LINE);
    /* list */
    IntersectClipRect(dc, la.l, la.t, la.r, la.b);
    const WCHAR* sec = 0;
    int chipW = sc(142);                        /* wide enough for the longest key name in this language */
    { WCHAR kn[64]; int w = tw(dc, fChip, TR(K_CHIP_PRESS)) + sc(20); if (w > chipW) chipW = w;
      for (i = 0; i < NG; i++) { pretty(groups[i].cur, kn); w = tw(dc, fChip, kn) + sc(20); if (w > chipW) chipW = w; }
      if (chipW > sc(230)) chipW = sc(230); }
    for (i = 0; i < NG; i++) {
        Group* g = &groups[i]; int y = la.t + g->y - g_scroll;
        if (!sec || !wieq(sec, g->sec)) { sec = g->sec;
            RECT h = { sc(36), y - sc(28), c.r - sc(28), y - sc(4) }; WCHAR up[64]; wcpy(up, sec); CharUpperW(up);
            if (h.b > la.t - sc(40) && h.t < la.b) txt(dc, up, h, C_MUTED, fSec, 0x20 | 4); }
        if (y + sc(40) < la.t || y > la.b) { memset(&g->chip, 0, sizeof g->chip); memset(&g->rowrc, 0, sizeof g->rowrc); continue; }
        int changed = g->cur[0] && !seq(g->cur, g->def), cap = (g_capture == i), hov = (g_hoverRow == i);
        RECT row = { sc(22), y, c.r - sc(28), y + sc(36) }; g->rowrc = row;
        if (hov || cap) rr(dc, row, sc(8), C_ROWH, 0, 0);
        if (changed) { RECT dot = { row.l + sc(6), row.t + sc(15), row.l + sc(12), row.t + sc(21) }; rr(dc, dot, sc(6), C_ORANGE, 0, 0); }
        RECT chip = { row.r - sc(8) - chipW, row.t + sc(4), row.r - sc(8), row.b - sc(4) }; g->chip = chip;
        RECT lt = { row.l + sc(20), row.t, chip.l - sc(10), row.b };
        txt(dc, g->label, lt, hov || cap ? C_JADE : C_TEAL, fRow, 0x20 | 4 | 0x8000);
        WCHAR kn[64];
        if (cap) { outline_key(dc, chip, (g_blink & 1) ? C_ORANGE : RGB(200,110,48), C_ORANGE, 1); txt(dc, TR(K_CHIP_PRESS), chip, C_DARK, fChip, 0x20 | 4 | 1); }
        else { pretty(g->cur, kn); outline_key(dc, chip, hov ? RGB(46,41,52) : C_CHIP, changed ? C_ORANGE : (hov ? C_JADE : C_CHIPB), sc(1) + (changed ? 1 : 0));
               txt(dc, kn, chip, g->cur[0] ? C_TEXT : C_MUTED, fChip, 0x20 | 4 | 1); }
    }
    SelectClipRgn(dc, 0);
    { RECT f1 = { 0, la.t, c.r, la.t + sc(8) }; dim(dc, f1, C_BG, 150); RECT f2 = { 0, la.b - sc(10), c.r, la.b }; dim(dc, f2, C_BG, 170); }
    /* scrollbar */
    int viewH = la.b - la.t; memset(&g_thumb, 0, sizeof g_thumb);
    if (g_contentH > viewH) { int th = viewH * viewH / g_contentH, ty = la.t + (viewH - th) * g_scroll / (g_contentH - viewH);
        RECT sb = { c.r - sc(14), ty, c.r - sc(10), ty + th }; g_thumb = sb; g_thumb.l -= sc(8); g_thumb.r += sc(4);
        rr(dc, sb, sc(4), g_drag ? C_TEAL : C_LINE, 0, 0); }
    /* info line: what the hovered key does, or the last message */
    r = c; r.t = la.b + sc(2); r.b = r.t + sc(1); r.l = sc(28); r.r -= sc(28); fill(dc, r, C_LINE);
    {
        WCHAR info[400]; u32 col = C_JADE; info[0] = 0;
        if (g_capture >= 0) { tf(info, 400, K_CAPTURE, groups[g_capture].label, 0, 0, 0); col = C_ORANGE; }
        else if (g_status[0] && (g_statusCol == C_WARN || GetTickCount() - g_statusAt < 3500)) { wcpy(info, g_status); col = g_statusCol; }
        else if (g_hoverRow >= 0 && !m_open) { Group* g = &groups[g_hoverRow]; wcpy(info, g->desc);
            if (g->cur[0] && !seq(g->cur, g->def)) { WCHAR dn[64], h2[200]; pretty(g->def, dn); tf(h2, 200, K_RESET_HINT, dn, 0, 0, 0); wcat(info, h2); }
            col = C_TEXT; }
        else { wcpy(info, TR(K_IDLE)); col = C_MUTED; }
        r = c; r.l = sc(28); r.r -= sc(28); r.t = la.b + sc(10); r.b = la.b + sc(68); txt(dc, info, r, col, fSub, 0x10);
    }
    /* buttons: Hob-style text buttons */
    int bw[NB] = { 104, 0, 112, 104 }, by = c.b - sc(56), bx = sc(28);
    for (i = 0; i < NB; i++) {
        if (i == B_SAVECLOSE) { memset(&btnRc[i], 0, sizeof btnRc[i]); continue; }
        int wpx = sc(bw[i]), need = tw(dc, fBtn, btnText[i]) + sc(30); if (need > wpx) wpx = need;
        int x = (i == B_CLOSE) ? c.r - sc(28) - wpx : bx;
        RECT b = { x, by, x + wpx, by + sc(38) }; btnRc[i] = b;
        int hb = g_hoverBtn == i, primary = (i == B_SAVE) && g_dirty;
        outline_key(dc, b, primary ? (hb ? RGB(250,160,90) : C_ORANGE) : (hb ? C_ROWH : C_BG), primary ? C_ORANGE : (hb ? C_JADE : C_LINE), 1);
        txt(dc, btnText[i], b, primary ? C_DARK : (hb ? C_JADE : C_TEAL), fBtn, 0x20 | 4 | 1);
        if (i != B_CLOSE) bx = b.r + sc(10);
    }
    /* dialog */
    if (m_open) {
        dim(dc, c, RGB(10,8,12), 190);
        int w = sc(460), h, need = sc(56) + sc(10) * (m_n - 1);
        for (i = 0; i < m_n; i++) { int bwn = tw(dc, fBtn, m_btn[i]) + sc(30); if (bwn < sc(110)) bwn = sc(110); need += bwn; }
        if (need > w) w = need; if (w > c.r - sc(24)) w = c.r - sc(24);
        { RECT mb = { 0, 0, w - sc(56), 0 }; SelectObject(dc, fSub); DrawTextW(dc, m_body, -1, &mb, 0x10 | 0x400 | 0x800 | (g_rtl ? 0x20000 : 0));
          h = sc(62) + (mb.b - mb.t) + sc(84); if (h < sc(240)) h = sc(240); if (h > c.b - sc(24)) h = c.b - sc(24); }
        RECT p = { (c.r - w) / 2, (c.b - h) / 2, (c.r - w) / 2 + w, (c.b - h) / 2 + h };
        rr(dc, p, sc(12), C_BG, C_LINE, sc(1));
        RECT t = { p.l + sc(28), p.t + sc(20), p.r - sc(28), p.t + sc(56) }; txt(dc, m_title, t, C_JADE, fBtn, 0x20 | 4);
        RECT bd = { p.l + sc(28), p.t + sc(62), p.r - sc(28), p.b - sc(70) }; txt(dc, m_body, bd, C_TEAL, fSub, 0x10);
        int n = m_n, bw2 = (w - sc(56) - sc(10) * (n - 1)) / n;
        for (i = 0; i < n; i++) { RECT b = { p.l + sc(28) + i * (bw2 + sc(10)), p.b - sc(58), 0, p.b - sc(22) }; b.r = b.l + bw2; m_rc[i] = b;
            int hb = m_hover == i;
            outline_key(dc, b, i == 0 ? (hb ? RGB(250,160,90) : C_ORANGE) : (hb ? C_ROWH : C_BG), i == 0 ? C_ORANGE : (hb ? C_JADE : C_LINE), 1);
            txt(dc, m_btn[i], b, i == 0 ? C_DARK : (hb ? C_JADE : C_TEAL), fBtn, 0x20 | 4 | 1); }
    }
}

static int same_ctx(Group* a, Group* b) { return seq(a->ctx, b->ctx); }
static void open_modal(int kind, const WCHAR* title, const WCHAR* body, int n, const WCHAR* b0, const WCHAR* b1, const WCHAR* b2) {
    m_open = 1; m_kind = kind; wcpy(m_title, title); wcpy(m_body, body); m_n = n; m_btn[0] = b0; m_btn[1] = b1; m_btn[2] = b2; m_hover = -1;
    g_ignoreUntil = GetTickCount() + 250; InvalidateRect(g_wnd, 0, 0);
}
static void bind_done(Group* g, const WCHAR* how) {
    WCHAR s[300], k[64]; pretty(g->cur, k); wcpy(s, g->label); wcat(s, L(" \x2192 ")); wcat(s, k); wcat(s, how ? how : TR(K_KEEP));
    status(s, C_TEAL);
}
static int is_fkey(const char* t) { return t[0] == 'F' && t[1] >= '1' && t[1] <= '9'; }
static int is_wheel(const char* t) { return seq(t, "MOUSE_WHEEL_UP") || seq(t, "MOUSE_WHEEL_DOWN"); }
static int can_take(Group* g, const char* t) { return is_mod(g) ? !is_wheel(t) : !is_fkey(t); }
static int m_noswap;
/* try to give group gi the token; asks if another action in the same context already uses it */
static void request_bind(int gi, const char* tok) {
    Group* g = &groups[gi]; int i;
    if (seq(g->cur, tok)) { WCHAR s[300], k[64]; pretty(tok, k); tf(s, 300, K_ALREADY, g->label, k, 0, 0); status(s, C_MUTED); return; }
    int nc = 0; WCHAR who[200]; who[0] = 0;
    for (i = 0; i < NG; i++) if (i != gi && same_ctx(&groups[i], g) && seq(groups[i].cur, tok)) {
        m_clash[nc++] = i; if (who[0]) wcat(who, L(", ")); wcat(who, groups[i].label); }
    if (!nc) { apply(g, tok); bind_done(g, 0); return; }
    m_group = gi; i = slen(tok); memcpy(m_tok, tok, i + 1); m_clash[nc] = -1;
    WCHAR k[64], ok[64], body[600], ttl[200]; pretty(tok, k); pretty(g->cur, ok);
    tf(ttl, 200, K_INUSE, k, 0, 0, 0);
    m_noswap = !g->cur[0];
    for (i = 0; m_clash[i] >= 0; i++) if (!can_take(&groups[m_clash[i]], g->cur)) m_noswap = 1;
    if (m_noswap) {
        tf(body, 600, K_CLASH_NOSWAP, who, k, g->label, ok);
        open_modal(MK_CLASH, ttl, body, 2, TR(K_B_BOTH), TR(K_B_CANCEL), 0); return; }
    tf(body, 600, K_CLASH_SWAP, who, k, g->label, ok);
    open_modal(MK_CLASH, ttl, body, 3, TR(K_B_SWAP), TR(K_B_BOTH), TR(K_B_CANCEL));
}
static int do_save(void) {
    int i, ok = 1;
    if (!nfiles) { status(TR(K_NO_FILE), C_WARN); return 0; }
    if (hob_running()) { status(TR(K_CLOSE_HOB), C_WARN); return 0; }
    for (i = 0; i < nfiles; i++) ok &= save_file(&files[i]);
    ok &= save_ini();
    if (!ok) { status(TR(K_WRITE_FAIL), C_WARN); return 0; }
    g_dirty = 0; status(TR(K_SAVED), C_TEAL); return 1;
}
static void modal_choice(int b) {
    int i; m_open = 0;
    if (m_kind == MK_CLASH) {
        Group* g = &groups[m_group];
        if (m_noswap) b++;                       /* dialog had no Swap button */
        if (b == 2) { status(TR(K_CANCELLED), C_MUTED); }
        else { char old[24]; memcpy(old, g->cur, 24); apply(g, m_tok);
            if (b == 0 && old[0]) { for (i = 0; m_clash[i] >= 0; i++) apply(&groups[m_clash[i]], old); bind_done(g, TR(K_SWAPPED)); }
            else bind_done(g, TR(K_SHARED)); }
    } else if (m_kind == MK_RESET) {
        if (b == 0) { for (i = 0; i < NG; i++) if (groups[i].cur[0] && !seq(groups[i].cur, groups[i].def)) apply(&groups[i], groups[i].def);
            status(TR(K_ALL_RESET), C_TEAL); }
    } else if (m_kind == MK_UNSAVED) {
        if (b == 0) { if (do_save()) DestroyWindow(g_wnd); }
        else if (b == 1) { g_dirty = 0; DestroyWindow(g_wnd); }
    }
    InvalidateRect(g_wnd, 0, 0);
}
static void try_close(void) {
    if (g_dirty) open_modal(MK_UNSAVED, TR(K_UNSAVED_T), TR(K_UNSAVED_B), 3, TR(K_B_SAVECLOSE), TR(K_B_DISCARD), TR(K_B_CANCEL));
    else DestroyWindow(g_wnd);
}
static int hit(RECT r, int x, int y) { return x >= r.l && x < r.r && y >= r.t && y < r.b; }
static void finish_capture(const char* tok) {
    int gi = g_capture; g_capture = -1;
    if (!tok) { status(TR(K_CANCELLED), C_MUTED); InvalidateRect(g_wnd, 0, 0); return; }
    request_bind(gi, tok); InvalidateRect(g_wnd, 0, 0);
}
static void clamp_scroll(void) { RECT la; list_area(&la); int mx = g_contentH - (la.b - la.t); if (mx < 0) mx = 0; if (g_scroll > mx) g_scroll = mx; if (g_scroll < 0) g_scroll = 0; }

static LRESULT WINAPI wndproc(HWND h, u32 msg, WPARAM wp, LPARAM lp) {
    int x = (short)(lp & 0xFFFF), y = (short)((lp >> 16) & 0xFFFF), i;
    switch (msg) {
    case 0x14: return 1; /* WM_ERASEBKGND */
    case 0x0F: { PAINTSTRUCT ps; HDC dc = BeginPaint(h, &ps); RECT c; GetClientRect(h, &c);
        HDC m = CreateCompatibleDC(dc); HBITMAP bm = CreateCompatibleBitmap(dc, c.r, c.b); HGDIOBJ ob = SelectObject(m, bm);
        draw(m); BitBlt(dc, 0, 0, c.r, c.b, m, 0, 0, 0x00CC0020);
        SelectObject(m, ob); DeleteObject(bm); DeleteDC(m); EndPaint(h, &ps); return 0; }
    case 0x113: g_blink++; InvalidateRect(h, 0, 0); return 0;
    case 0x20: if ((lp & 0xFFFF) == 1) { SetCursor(LoadCursorW(0, (g_hoverRow >= 0 || g_hoverBtn >= 0 || m_hover >= 0) ? (const WCHAR*)32649 : (const WCHAR*)32512)); return 1; } break;
    case 0x202: if (g_drag) { g_drag = 0; ReleaseCapture(); InvalidateRect(h, 0, 0); } return 0;
    case 0x200: { /* mouse move */
        if (g_drag) { RECT la; list_area(&la); int viewH = la.b - la.t, th = viewH * viewH / g_contentH;
            if (viewH - th > 0) g_scroll = g_dragS0 + (y - g_dragY0) * (g_contentH - viewH) / (viewH - th); clamp_scroll(); InvalidateRect(h, 0, 0); return 0; }
        TRACKMOUSEEVENT t = { sizeof t, 2, h, 0 }; TrackMouseEvent(&t);
        int hr = -1, hb = -1, hm = -1; RECT la; list_area(&la);
        if (m_open) { for (i = 0; i < m_n; i++) if (hit(m_rc[i], x, y)) hm = i; }
        else { if (hit(la, x, y)) for (i = 0; i < NG; i++) if (hit(groups[i].rowrc, x, y)) hr = i;
               for (i = 0; i < NB; i++) if (hit(btnRc[i], x, y)) hb = i; }
        if (hr != g_hoverRow || hb != g_hoverBtn || hm != m_hover) { g_hoverRow = hr; g_hoverBtn = hb; m_hover = hm; InvalidateRect(h, 0, 0); }
        return 0; }
    case 0x2A3: g_hoverRow = g_hoverBtn = m_hover = -1; InvalidateRect(h, 0, 0); return 0;
    case 0x100: case 0x104: { /* key down */
        if (GetTickCount() < g_ignoreUntil) return 0;
        if (m_open) { if (wp == 0x1B) modal_choice(m_n - 1); else if (wp == 0x0D) modal_choice(0); return 0; }
        if (g_capture >= 0) {
            if (wp == 0x1B) { finish_capture(0); return 0; }
            const char* t = token_for_vk((u32)wp);
            if (!t) { status(TR(K_NUMPAD), C_WARN); return 0; }
            if (!can_take(&groups[g_capture], t)) { status(TR(K_FKEYS), C_WARN); return 0; }
            finish_capture(t); return 0; }
        if (wp == 0x1B) { try_close(); return 0; }
        break; }
    case 0x106: case 0x112: if (msg == 0x106 || (wp & 0xFFF0) == 0xF100) return 0; break; /* no Alt menu */
    case 0x201: case 0x204: case 0x207: case 0x20B: {
        if (GetTickCount() < g_ignoreUntil) return 0;
        if (m_open) { if (msg == 0x201) for (i = 0; i < m_n; i++) if (hit(m_rc[i], x, y)) { modal_choice(i); break; } return 0; }
        if (g_capture >= 0) {
            const char* t = msg == 0x201 ? "MOUSE_LEFT" : msg == 0x204 ? "MOUSE_RIGHT" : msg == 0x207 ? "MOUSE_MIDDLE" : (((wp >> 16) & 0xFFFF) == 1 ? "MOUSE_BUTTON1" : "MOUSE_BUTTON2");
            finish_capture(t); return msg == 0x20B ? 1 : 0; }
        if (msg == 0x201 && g_thumb.b > g_thumb.t && hit(g_thumb, x, y)) { g_drag = 1; g_dragY0 = y; g_dragS0 = g_scroll; SetCapture(h); InvalidateRect(h, 0, 0); return 0; }
        RECT la; list_area(&la);
        if (hit(la, x, y)) for (i = 0; i < NG; i++) if (groups[i].cur[0] && hit(groups[i].rowrc, x, y)) {
            if (msg == 0x201) { g_capture = i; g_blink = 0; WCHAR s[300]; tf(s, 300, K_NEWKEY, groups[i].label, 0, 0, 0); status(s, C_TEAL); }
            else if (msg == 0x204) { if (seq(groups[i].cur, groups[i].def)) status(TR(K_IS_DEFAULT), C_MUTED); else request_bind(i, groups[i].def); }
            InvalidateRect(h, 0, 0); return 0; }
        if (msg == 0x201) for (i = 0; i < NB; i++) if (hit(btnRc[i], x, y)) {
            if (i == B_SAVE) do_save();
            else if (i == B_SAVECLOSE) { if (do_save()) DestroyWindow(h); }
            else if (i == B_RESET) open_modal(MK_RESET, TR(K_RESET_T), TR(K_RESET_B), 2, TR(K_B_RESET), TR(K_B_CANCEL), 0);
            else try_close();
            return 0; }
        return 0; }
    case 0x20A: { short d = (short)((wp >> 16) & 0xFFFF);
        if (GetTickCount() < g_ignoreUntil || m_open) return 0;
        if (g_capture >= 0) { if (is_mod(&groups[g_capture])) { status(TR(K_WHEEL), C_WARN); return 0; }
            finish_capture(d > 0 ? "MOUSE_WHEEL_UP" : "MOUSE_WHEEL_DOWN"); return 0; }
        g_scroll -= d * sc(44) / 120; clamp_scroll(); InvalidateRect(h, 0, 0); return 0; }
    case 0x10: try_close(); return 0;
    case 0x02: PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

IMP int WINAPI MultiByteToWideChar(u32, u32, const char*, int, WCHAR*, int);
/* translations live in "Hob Keybinds.lang" (UTF-8) next to the exe: [code] then id|text lines */
static void load_lang(const char* code) {
    WCHAR path[600]; int n, i; u32 sz, rd; HANDLE h; char* b; WCHAR* w; int insec = 0;
    n = (int)GetModuleFileNameW(0, path, 590); while (n > 0 && path[n - 1] != '\\') n--; path[n] = 0; wcat(path, L("Hob Keybinds.lang"));
    h = CreateFileW(path, 0x80000000, 1, 0, 3, 0x80, 0); if (h == (HANDLE)-1) return;
    sz = GetFileSize(h, 0); b = alloc(sz + 1); ReadFile(h, b, sz, &rd, 0); CloseHandle(h); b[rd] = 0;
    n = MultiByteToWideChar(65001, 0, b, (int)rd, 0, 0); w = alloc((n + 1) * 2); MultiByteToWideChar(65001, 0, b, (int)rd, w, n); w[n] = 0;
    for (i = 0; i < n; ) {
        WCHAR* line = w + i; int e = i; while (e < n && w[e] != '\n') e++; w[e] = 0; if (e > i && w[e - 1] == '\r') w[e - 1] = 0; i = e + 1;
        if (line[0] == 0xFEFF) line++;
        if (line[0] == '[') { int k = 0; insec = 1; while (code[k] && line[1 + k] == code[k]) k++; if (code[k] || line[1 + k] != ']') insec = 0; continue; }
        if (!insec) continue;
        { int k, bar = -1, id; for (k = 0; line[k]; k++) if (line[k] == '|') { bar = k; break; } if (bar <= 0) continue;
          for (id = 0; id < K__N; id++) { const char* a = g_kbIds[id]; int q = 0; while (a[q] && q < bar && line[q] == (WCHAR)a[q]) q++; if (!a[q] && q == bar) break; }
          if (id == K__N) continue;
          { WCHAR* t = line + bar + 1; WCHAR* d = t; WCHAR* r = t; while (*r) { if (r[0] == '\\' && r[1] == 'n') { *d++ = '\n'; r += 2; } else *d++ = *r++; } *d = 0; g_tr[id] = t; } }
    }
}
static void detect_lang(void) {      /* HobTools writes Hob's language (e.g. LANGUAGE=FRENCH) into HobTools.ini */
    CFile f; int i, k;
    g_lg = LG_EN; g_langName[0] = 0; wcpy(f.path, iniPath);
    if (load_file(&f)) for (i = 0; i < f.n; i++) { const char* t = trim(f.line[i]);
        if (seq8(t, "LANGUAGE=")) { for (k = 0; k < 31 && t[9 + k] && t[9 + k] != ' ' && t[9 + k] != '\r'; k++) g_langName[k] = t[9 + k]; g_langName[k] = 0;
            g_lg = lang_from_name(g_langName); } }
    g_rtl = g_lg == LG_AR;
    if (g_lg != LG_EN) load_lang(g_langCodes[g_lg]);   /* no "Hob Keybinds.lang" shipped in 1.0 -> English */
}
static HFONT font(int px, int weight, const WCHAR* face) { return CreateFontW(-sc(px), 0, 0, 0, weight, 0, 0, 0, 1, 0, 0, 5, 0, face); }

void start(void) {
    int i;
    SetProcessDPIAware();
    { HDC dc = GetDC(0); S = GetDeviceCaps(dc, 90) / 96.0f; ReleaseDC(0, dc); if (S < 1) S = 1; }
    SHGetFolderPathW(0, 5, 0, 0, hobDir); wcat(hobDir, L("\\My Games\\Runic Games\\HOB"));
    wcpy(iniPath, hobDir); wcat(iniPath, L("\\HobTools.ini"));   /* next to Hob's own settings, outside the game folder */
    detect_lang();
    { const WCHAR* fr = L("Bahnschrift"); const WCHAR* fb = L("Bahnschrift SemiBold"); int wb = 600;   /* Bahnschrift has no CJK, Cyrillic or Arabic */
      if (g_lg == LG_RU || g_lg == LG_AR) { fr = L("Segoe UI"); fb = L("Segoe UI Semibold"); }
      else if (g_lg == LG_JA) { fr = L("Yu Gothic UI"); fb = L("Yu Gothic UI Semibold"); }
      else if (g_lg == LG_KO) { fr = fb = L("Malgun Gothic"); wb = 700; }
      else if (g_lg == LG_ZH_CN) { fr = fb = L("Microsoft YaHei UI"); wb = 700; }
      else if (g_lg == LG_ZH_TW) { fr = fb = L("Microsoft JhengHei UI"); wb = 700; }
      fTitle = font(30, wb, fb); fSub = font(15, 400, fr); fRow = font(18, 400, fr);
      fChip = font(15, wb, fb); fSec = font(13, wb, fb); fBtn = font(17, wb, fb); }
    btnText[B_SAVE] = TR(K_B_SAVE); btnText[B_SAVECLOSE] = TR(K_B_SAVECLOSE); btnText[B_RESET] = TR(K_B_RESET); btnText[B_CLOSE] = TR(K_B_CLOSE);

    for (i = 0; i < NG; i++) { groups[i].sec = TR(groups[i].secId); groups[i].label = TR(groups[i].labelId); groups[i].desc = TR(groups[i].descId); }
    load_all(); load_ini(); layout();
    if (!nfiles) { wcpy(g_status, TR(K_NO_FILE)); g_statusCol = C_WARN; }
    else if (hob_running()) { wcpy(g_status, TR(K_RUNNING)); g_statusCol = C_WARN; }
    else g_status[0] = 0;

    HINSTANCE inst = GetModuleHandleW(0);
    WNDCLASSEXW wc; memset(&wc, 0, sizeof wc); wc.cbSize = sizeof wc; wc.proc = wndproc; wc.inst = inst;
    wc.cur = LoadCursorW(0, (const WCHAR*)32512); wc.name = L("HobKeybinds"); wc.style = 3;
    wc.icon = LoadIconW(inst, (const WCHAR*)1); wc.iconSm = wc.icon;
    RegisterClassExW(&wc);
    u32 style = 0x00CA0000; RECT r = { 0, 0, sc(580), sc(820) };
    int sh = GetSystemMetrics(1) - sc(60); if (r.b > sh) r.b = sh;
    AdjustWindowRectEx(&r, style, 0, 0);
    g_wnd = CreateWindowExW(0, L("HobKeybinds"), L("Hob Keybinds"), style, (int)0x80000000, (int)0x80000000, r.r - r.l, r.b - r.t, 0, 0, inst, 0);
    { BOOL dark = 1; DwmSetWindowAttribute(g_wnd, 20, &dark, 4); }
    SetTimer(g_wnd, 1, 450, 0);
    ShowWindow(g_wnd, 1); UpdateWindow(g_wnd);
    MSG m; while (GetMessageW(&m, 0, 0, 0) > 0) { TranslateMessage(&m); DispatchMessageW(&m); }
    ExitProcess(0);
}
