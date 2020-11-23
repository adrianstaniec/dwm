/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 10;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 10;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "UbuntuMono Nerd Font Mono:pixelsize=18:antialias=true:autohint=true" };
static const char dmenufont[]       = "Liberation Mono:pixelsize=17:antialias=true:autohint=true";
static char normbgcolor[]           = "#222222";
static char normfgcolor[]           = "#bbbbbb";
static char normbordercolor[]       = "#444444";
static char normfloatbordercolor[]  = "#555555";
static char normstickybordercolor[] = "#666666";
static char selbgcolor[]            = "#005577";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selfloatbordercolor[]   = "#335577";
static char selstickybordercolor[]  = "#337799";
static char *colors[][5] = {
       /*               fg           bg           border           float border           sticky border         */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor, normfloatbordercolor,  normstickybordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor , selfloatbordercolor ,  selstickybordercolor},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class       instance  title           tags mask  isfloating  monitor */
	{ "Audacious", NULL,     NULL,           1 << 8,    0,          -1 },
	{ "Gimp",      NULL,     NULL,           0,         1,          -1 },
	{ "St",        NULL,     NULL,           0,         0,          -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },
#define STACKKEYS(MOD,ACTION) \
	{ MOD, XK_j,     ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_k,     ACTION##stack, {.i = INC(-1) } }, \
	{ MOD, XK_Down,  ACTION##stack, {.i = INC(+1) } }, \
	{ MOD, XK_Up,    ACTION##stack, {.i = INC(-1) } },


/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]      = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *termcmd[]       = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };
/* static const char *rofi_xdg[]   = { "rofi-xdg_open.sh", "~/Documents", NULL }; */
static const char* vk[]            = { "svkbd-mobile-intl", NULL };
static const char* dummy[]         = { "st", "-e", "bat", "/github/suckless/dwm/config.h", NULL };

static const Launcher launchers[] = {
	/* command    name to display */
	{ dummy,      "|"},
	{ dmenucmd,   "" },
	{ termcmd,    "" },
	{ vk,         "" },
};


static Key keys[] = {
	/* modifier                     key        function        argument */
  /* gaps */
	{ MODKEY|Mod1Mask,              XK_h,      incrgaps,       {.i = +1 } },
	{ MODKEY|Mod1Mask,              XK_l,      incrgaps,       {.i = -1 } },
	{ MODKEY|Mod1Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
  /* top row */
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
  /* qwerty row */
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} }, 
	{ MODKEY|ShiftMask,             XK_w,      spawn,          SHCMD("~/bin/set-random-wallpaper.sh") },
	{ MODKEY,                       XK_e,      spawn,          SHCMD("st -e nvim") },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          SHCMD("code") },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_t,      spawn,          SHCMD("~/bin/set-theme2.sh") },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      spawn,          SHCMD("~/bin/rofi-nerd.sh 1") },
	{ MODKEY,                       XK_o,      spawn,          SHCMD("~/bin/rofi-open-pdf.sh ~/Dropbox/Books") },
	{ MODKEY|ShiftMask,             XK_o,      spawn,          SHCMD("rofi-xdg-open.sh ~/projects/") },
	{ MODKEY,                       XK_p,      spawn,          SHCMD("~/bin/rofi-open-pdf.sh ~/Dropbox/Papers") },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          SHCMD("~/bin/getpaper.py") },
  /* asdf row */
	{ MODKEY,                       XK_a,      incnmaster,     {.i = 1 } },
	{ MODKEY,                       XK_s,      spawn,          SHCMD("~/bin/greenshot.sh") },
	{ MODKEY|ShiftMask,             XK_s,      togglesticky,   {0} },
	{ MODKEY,                       XK_d,      spawn,          SHCMD("rofi -show run") },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_g,      spawn,          SHCMD("~/bin/rofi-surfraw-google.sh") },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          SHCMD("~/bin/rofi-surfraw-websearch.sh") },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
  /* zxcv row */
	{ MODKEY,                       XK_x,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_c,      spawn,          SHCMD("~/bin/rofi-config-bookmarks.sh") },
	{ MODKEY|ShiftMask,             XK_c,      spawn,          SHCMD("~/bin/toggle-compositor.sh") },
	{ MODKEY,                       XK_b,      spawn,          SHCMD("~/bin/rofi-surfraw-bookmarks.sh") },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
  /* bottom row */
/* { MODKEY,                       XK_space,  setlayout,      {0} }, */
	{ MODKEY,                       XK_space,  zoom,           {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_F5,     xrdb,           {.v = NULL } },

  /* multimedia */
  // pactl
	/* { 0,                            XF86XK_AudioRaiseVolume,   spawn,  SHCMD("pactl set-sink-volume 0 +5%")}, */
	/* { 0,                            XF86XK_AudioLowerVolume,   spawn,  SHCMD("pactl set-sink-volume 0 -5%")}, */
	/* { 0,                            XF86XK_AudioMute,          spawn,  SHCMD("pactl set-sink-mute 0 toggle")}, */
  // amixer
	/* { 0,                            XF86XK_AudioRaiseVolume,   spawn,  SHCMD("amixer -D pulse sset Master 5%+")}, */
	/* { 0,                            XF86XK_AudioLowerVolume,   spawn,  SHCMD("amixer -D pulse sset Master 5%-")}, */
  // pamixer
	{ MODKEY,                       XK_equal,                  spawn,  SHCMD("pamixer --allow-boost -i 5")},
	{ MODKEY,                       XK_minus,                  spawn,  SHCMD("pamixer --allow-boost -d 5")},
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,  SHCMD("pamixer --allow-boost -i 5")},
	{ 0,                            XF86XK_AudioLowerVolume,   spawn,  SHCMD("pamixer --allow-boost -d 5")},
	{ 0,                            XF86XK_AudioMute,          spawn,  SHCMD("pamixer -t")},
  // player
	{ 0,                            XF86XK_AudioPrev,          spawn,  SHCMD("audacious -r")},
	{ 0,                            XF86XK_AudioNext,          spawn,  SHCMD("audacious -f")},
	{ 0,                            XF86XK_AudioPlay,          spawn,  SHCMD("audacious -t")},
	{ 0,                            XF86XK_AudioStop,          spawn,  SHCMD("audacious -s")},
  /* tagkeys */
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
  /* stacker */
	STACKKEYS(MODKEY,                          focus)
	STACKKEYS(MODKEY|ShiftMask,                push)

};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

