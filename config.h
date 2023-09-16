/* See LICENSE file for copyright and license details. */

#include "fibonacci.c"
#include "horizgrid.c"
#include "layouts.c"
#include "movestack.c"

/* appearance */
static const unsigned int borderpx  = 0;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */

static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/

static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */

static const int showsystray        = 1;     /* 0 means no systray */

static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */

static const char *fonts[]          = { "open sans:size=9", "Font Awesome 5 Pro:size=9" };

static const char col_gray1[]       = "#1c1c1c";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#1c1c1c";

static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;
const char *spcmd1[] = {"urxvtc", "-name", "sp-tmux", "-geometry", "230x60", "-e", "scratch-tmux", NULL };
const char *spcmd2[] = {"urxvtc", "-name", "sp-lf",   "-geometry", "144x41", "-e", "lf", NULL };
const char *spcmd3[] = {"urxvtc", "-name", "sp-math", "-geometry", "100x40", "-e", "scratch-math", NULL };
const char *spcmd4[] = {"caprine", NULL };
const char *spcmd5[] = {"ferdi", NULL };
static Sp scratchpads[] = {
	/* name          cmd  */
	{"sp-tmux",      spcmd1},
	{"sp-lf",        spcmd2},
	{"sp-math",      spcmd3},
	{"caprine",      spcmd4},
	{"ferdi",        spcmd5},
};

/* tagging */
static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const char *tagsalt[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title             tags mask     isfloating  isterminal   noswallow  monitor  */
	{ "Gimp",     NULL,       NULL,             2,            0,          0,           0,         -1 },
	{ "Firefox",  NULL,       NULL,             1 << 8,       0,          0,           0,         -1 },
	{ NULL,       "Spotify",  NULL,             7,            1,          0,           0,         -1 },
	{ "Gimp",	  NULL,		  NULL,	            0,			  1,		  0,           0,         -1 },
	{ "Firefox",  NULL,		  NULL,	            1 << 8,		  0,		  0,           0,         -1 },
    { "St",       NULL,       NULL,       	    0,            0,          1,           0,         -1 },
    { "URxvt",    NULL,       NULL,       	    0,            0,          1,           1,         -1 },
	{ NULL,       NULL,       "Event Tester",   0,            0,          0,           1,         -1 },
	{ NULL,		  "sp-tmux",  NULL,	            SPTAG(0),	  1,		  0,           1,         -1 },
	{ NULL,		  "sp-lf",	  NULL,	            SPTAG(1),	  1,		  0,           1,         -1 },
	{ NULL,		  "sp-math",  NULL,	            SPTAG(2),	  1,		  0,           1,         -1 },
	{ NULL,		  "caprine",  NULL,	            SPTAG(3),	  1,		  0,           1,         -1 },
	{ NULL,		  "ferdi",    NULL,	            SPTAG(4),	  1,		  0,           1,         -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "[M]",      monocle },
	{ "HHH",      grid },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
    { "[\\]",      dwindle },
	{ "TTT",      bstack },
	{ "[D]",      deck },
	{ "===",      bstackhoriz },
	{ "|||",      col },
    { "###",      horizgrid },
 	{ "[@]",      spiral },
	{ "><>",      NULL },    /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define ALTKEY Mod1Mask

#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */

static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_c,        killclient,     {0} },

 	{ MODKEY,            			XK_t,  	     togglescratch,  {.ui = 0 } },
 	{ MODKEY,            			XK_r,	     togglescratch,  {.ui = 1 } },
 	{ MODKEY|ShiftMask,   			XK_r,	     togglescratch,  {.ui = 2 } },
 	{ MODKEY|ShiftMask,   			XK_m,	     togglescratch,  {.ui = 3 } },
 	{ MODKEY|ShiftMask,   			XK_f,	     togglescratch,  {.ui = 4 } },

	{ MODKEY|ShiftMask,             XK_t,        togglebar,      {0} },

	{ MODKEY|ShiftMask,             XK_j,        movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,        movestack,      {.i = -1 } },

	{ MODKEY|ShiftMask,		        XK_h,		 incnmaster,	 {.i = +1 } },
	{ MODKEY|ShiftMask,		        XK_l,		 incnmaster,	 {.i = -1 } },

	{ MODKEY,                       XK_j,        focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,        focusstack,     {.i = -1 } },

	{ MODKEY,                       XK_h,        setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,        setmfact,       {.f = +0.05} },

	{ MODKEY,                       XK_space,    zoom,           {0} },

	{ ALTKEY,                       XK_Tab,      view,           {0} },

	{ MODKEY,                       XK_f,        setlayout,      {0} },

	{ ALTKEY,                       XK_1,        setlayout,      {.v = &layouts[0]} },
	{ ALTKEY,                       XK_2,        setlayout,      {.v = &layouts[1]} },
	{ ALTKEY,                       XK_3,        setlayout,      {.v = &layouts[2]} },
	{ ALTKEY,                       XK_4,        setlayout,      {.v = &layouts[3]} },
	{ ALTKEY,                       XK_5,        setlayout,      {.v = &layouts[4]} },
	{ ALTKEY,                       XK_6,        setlayout,      {.v = &layouts[5]} },
	{ ALTKEY,                       XK_7,        setlayout,      {.v = &layouts[6]} },
	{ ALTKEY,                       XK_8,        setlayout,      {.v = &layouts[7]} },
	{ ALTKEY,                       XK_9,        setlayout,      {.v = &layouts[8]} },
	{ ALTKEY,                       XK_0,        setlayout,      {.v = &layouts[8]} },

	{ MODKEY,                       XK_s,        togglesticky,   {0} },
	{ MODKEY|ShiftMask,             XK_space,    togglefloating, {0} },

	{ MODKEY,                       XK_0,        view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,        tag,            {.ui = ~0 } },

	{ MODKEY,                       XK_i,        focusmon,       {.i = +1 } },
	{ MODKEY,                       XK_u,        focusmon,       {.i = -1 } },

	{ MODKEY|ShiftMask,             XK_i,        tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_u,        tagmon,         {.i = -1 } },

	{ ALTKEY,                       XK_grave,      togglealttag,   {0} },

    { MODKEY|ShiftMask,             XK_p,        shiftview,      { .i = -1 } },
    { MODKEY|ShiftMask,             XK_n,        shiftview,      { .i = 1 } },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)

	{ ALTKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

