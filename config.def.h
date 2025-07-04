/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* /1* purple-pink skin *1/ */
/* #define COLOR_BG "#220022" */
/* #define COLOR_FG "#eeeeee" */
/* #define COLOR_SEL_BG "#cc0077" */
/* #define COLOR_SEL_FG "#ffff00" */
/* #define COLOR_SEL_FG2 "#000000" */

/* grey-orange skin */
#define COLOR_BG "#222222"
#define COLOR_FG "#eeeeee"
#define COLOR_SEL_BG "#cc7700"
#define COLOR_SEL_FG "#000000"
#define COLOR_SEL_FG2 "#000000"

static const char col_bar_bg[]     = COLOR_BG;
static const char col_bar_fg[]     = COLOR_FG;
static const char col_bar_sel_bg[] = COLOR_SEL_BG;
static const char col_bar_sel_fg[] = COLOR_SEL_FG;

static const char col_border[]     = COLOR_BG;
static const char col_border_sel[] = COLOR_SEL_BG;

static const char col_dmenu_bg[]     = COLOR_BG;
static const char col_dmenu_fg[]     = COLOR_FG;
static const char col_dmenu_sel_bg[] = COLOR_SEL_BG;
static const char col_dmenu_sel_fg[] = COLOR_SEL_FG2;
static const char col_dmenu_border[] = COLOR_SEL_BG;

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 1;        /* snap pixel */
static const unsigned int systraypinning = -1;  /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X, -1: show on every monitor */
static const unsigned int systrayonleft = 1;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "Go Mono:size=10", "Inconsolata Nerd Font Mono:size=20" };
static const char dmenufont[]       = "Inconsolata Nerd Font Mono:size=24";

static const char *colors[][3]      = {
	/*               fg               bg               border   */
	[SchemeNorm] = { col_bar_fg,      col_bar_bg,      col_border      },
	[SchemeSel]  = { col_bar_sel_fg,  col_bar_sel_bg,  col_border_sel  },
};

/* tagging */
static const char *tags[] = { "1 ", "2 ", "3 󰒱", "4 󰈹", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class            instance        title                   tags mask   isfloating   monitor */

	/* no tags */
	{ "Gimp",          NULL,            NULL,                   0,          1,           -1 },
	{ "Conky",         "Conky",         NULL,                   0,          0,           0 },

	/* tag 2: work web browser */
	{ "Google-chrome", "google-chrome", NULL,                   1 << 1,     0,           -1 },

	/* tag 3: work chat */
	{ "Slack",         "slack",         NULL,                   1 << 2,     0,           -1 },

	/* tag 4: work chat */
	{ "Firefox",       NULL,            NULL,                   1 << 3,     0,           -1 },
	{ "firefox-esr",   "Navigator",     NULL,                   1 << 3,     0,           -1 },
	{ "librewolf",     "Navigator",     NULL,                   1 << 3,     0,           -1 },

	/* all tags: picture in picture */
	{ "firefox-esr",   "Toolkit",       "Picture-in-Picture",   ~0,         1,           -1 },
	{ "librewolf",     "Toolkit",       "Picture-in-Picture",   ~0,         1,           -1 },

	/* tag 5: entertainment */
	{ "vlc",           "vlc",           NULL,                   1 << 4,     1,           -1 },
	{ "Spotify",       "spotify",       NULL,                   1 << 4,     1,           -1 },

	/* tag 6: zoom */
	{ "zoom",          "zoom",          "Zoom Meeting",         1 << 5,     0,           -1 },
	{ "zoom",          "zoom",          "Zoom Cloud Meetings",  1 << 5,     1,           -1 },

	/* tag 7: empty */

	/* tag 8: todo list */
	{ "st-256color",   "st-256color",   "godo",                 1 << 7,     0,           -1 },

	/* tag 9: clock wall */
	{ "st-256color",   "st-256color",   "clock",                1 << 8,     0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },        /* first entry is default */
	{ "><>",      NULL },        /* no layout function means floating behavior */
	{ "[M]",      monocle },     /* only one window in the tag focus on other window to show it */
	{ "HHH",      gaplessgrid }, /* arrenge windows in a grid, no master */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]      = { "dmenu_run", "-p", "run:", "-i", "-b", "-m", dmenumon, "-fn", dmenufont, "-bo", col_dmenu_border, "-nb", col_dmenu_bg, "-nf", col_dmenu_fg, "-sb", col_dmenu_sel_bg, "-sf", col_dmenu_sel_fg, NULL };
static const char *termcmd[]       = { "st", "-e", "tmux", NULL };
static const char *slockcmd[]      = { "slock", NULL };
static const char *powercmd[]      = { "power-menu", NULL };
static const char *upvol[]         = { "/usr/bin/wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%+",    NULL };
static const char *downvol[]       = { "/usr/bin/wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%-",    NULL };
static const char *mutevol[]       = { "/usr/bin/wpctl", "set-mute",   "@DEFAULT_AUDIO_SINK@", "toggle", NULL };
static const char *upbright[]      = { "/usr/bin/brightnessctl", "set", "10%+", NULL };
static const char *downbright[]    = { "/usr/bin/brightnessctl", "set", "10%-", NULL };
static const char *screenshot[]    = { "flameshot", "full", NULL };
static const char *screenshotsel[] = { "flameshot", "gui", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_space,  spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_t,      spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = slockcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_p,      setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_p,      togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      spawn,          {.v = powercmd} },
	{ MODKEY|ShiftMask,             XK_c,      quit,           {0} },

	/* Print whole screen or select region, with 0 is just the key directly */
	{ 0,      XK_Print, spawn, {.v = screenshot    } },
	{ MODKEY, XK_Print, spawn, {.v = screenshotsel } },

	/* Keyboard controls */
	{ 0, XF86XK_AudioRaiseVolume,  spawn, {.v = upvol      } },
	{ 0, XF86XK_AudioLowerVolume,  spawn, {.v = downvol    } },
	{ 0, XF86XK_AudioMute,         spawn, {.v = mutevol    } },
	{ 0, XF86XK_MonBrightnessUp,   spawn, {.v = upbright   } },
	{ 0, XF86XK_MonBrightnessDown, spawn, {.v = downbright } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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

