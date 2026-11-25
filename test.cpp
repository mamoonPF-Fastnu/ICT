/* ======================================================================
 *  LIFE RPG  -  the whole project in ONE file
 *
 *  This file is the original multi-file project (types.h, character, tasks,
 *  boss, storage, game, ui, main) joined together in this order:
 *     1. shared types   2. function list (prototypes)   3. the code   4. main
 *
 *  Build:  gcc -std=c99 liferpg_all.c -o liferpg        (Windows: -o liferpg.exe)
 *  Run:    ./liferpg                                    (Windows: liferpg.exe)
 * ====================================================================== */

#define _POSIX_C_SOURCE 200809L   /* for fileno() under -std=c99 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <io.h>
#define IS_TTY(f) _isatty(_fileno(f))
#else
#include <unistd.h>
#define IS_TTY(f) isatty(fileno(f))
#endif

#define SAVE_FILE "liferpg.sav"


/* ==================================================================
 * 1. SHARED TYPES  (was include/types.h)
 * ================================================================== */

/* types.h -- SHARED CONTRACT. Owned by the team lead.
 * Every module includes this file. Change it only after telling the team,
 * because a change here can break everybody's code. */

#define MAX_NAME        32
#define MAX_TASK_NAME   48
#define MAX_TASKS       20
#define XP_PER_MINUTE   1     /* 60 XP per hour of effort            */
#define LEVEL_COST_BASE 50    /* XP to go from level L to L+1 = 50*L */
#define TITLE_TIER_SIZE 5     /* new title every 5 levels            */

typedef enum {
    SKILL_STUDY, SKILL_FITNESS, SKILL_CREATIVE, SKILL_SOCIAL,
    SKILL_COUNT
} Skill;

typedef struct { int xp; } SkillStat;            /* level is derived from xp */

typedef struct {
    char      name[MAX_NAME];
    SkillStat skills[SKILL_COUNT];
    int       total_xp;
    int       bosses_defeated;
} Character;

typedef struct {
    char  name[MAX_TASK_NAME];
    Skill skill;
    int   target_min;   /* planned minutes this week */
    int   logged_min;   /* minutes done so far       */
    int   done;         /* 1 when logged >= target   */
} Task;

typedef struct { Task items[MAX_TASKS]; int count; } TaskList;

typedef struct {
    char  name[MAX_TASK_NAME];
    Skill skill;         /* only effort in this skill damages the boss */
    int   required_min;  /* "HP" of the boss                           */
    int   progress_min;
    int   reward_xp;
    int   defeated;
} Boss;

typedef struct {
    Character hero;
    TaskList  tasks;
    Boss      boss;
    long      week;      /* weeks since 1970; used to detect a new week */
} GameState;

/* Result of giving XP to a skill (so the UI can celebrate). */
typedef struct {
    Skill       skill;
    int         level_before, level_after, levels_gained;
    int         title_unlocked;   /* 1 if a new title tier was reached */
    const char *title;            /* the skill's current title         */
} XpResult;

/* Everything that happened when the player logged effort. */
typedef struct {
    XpResult task_xp;
    int      boss_defeated;       /* 1 if this log finished the boss   */
    XpResult boss_xp;             /* valid only if boss_defeated       */
} LogOutcome;



/* ==================================================================
 * 2. FUNCTION LIST  (was the other header files)
 * ================================================================== */

/* --- character --- */
/* MODULE 1 -- character : hero profile, XP, levels, titles */

void        character_init(Character *c, const char *name);
const char *skill_name(Skill s);

int         xp_to_level(int xp);                         /* level for an XP total  */
void        level_progress(int xp, int *into, int *need);/* XP into level / needed */
XpResult    character_add_xp(Character *c, Skill s, int xp);
int         character_level(const Character *c);         /* overall level          */
Skill       character_best_skill(const Character *c);
const char *character_title(const Character *c);         /* title of best skill    */

/* --- tasks --- */
/* MODULE 2 -- tasks : the weekly task list */

void tasks_init(TaskList *t);
int  tasks_add(TaskList *t, const char *name, Skill s, int target_min); /* index or -1 */
int  tasks_log(TaskList *t, int index, int minutes);   /* 0 ok, -1 invalid input */
int  tasks_remove(TaskList *t, int index);             /* 0 ok, -1 invalid       */
void tasks_reset_week(TaskList *t);                    /* clear logged/done      */

/* --- boss --- */
/* MODULE 3 -- boss : the Weekly Boss Battle */

void boss_spawn(Boss *b, long week);   /* same week => same boss for everyone */
int  boss_apply_progress(Boss *b, Skill s, int minutes); /* 1 if just defeated */
int  boss_percent(const Boss *b);      /* 0..100 of the boss's HP removed      */

/* --- storage --- */
/* MODULE 4 -- storage : save and load the game (plain text file) */

int storage_save(const GameState *g, const char *path);  /* 0 ok, -1 error        */
int storage_load(GameState *g, const char *path);        /* 0 ok, -1 none/corrupt */

/* --- game --- */
/* GLUE -- game : connects the modules (shared by the whole team) */

long game_current_week(void);
void game_init(GameState *g, const char *hero_name, long week);
int  game_sync_week(GameState *g, long week);   /* 1 if a new week started */
int  game_log_effort(GameState *g, int task_index, int minutes, LogOutcome *out);

/* --- ui --- */
/* MODULE 5 -- ui : everything printed to or read from the terminal.
 * v2: colours, boxes, title screen. Falls back to plain ASCII when output is
 * not a terminal or the NO_COLOR environment variable is set. */

void  ui_init(void);                      /* call once at startup            */
void  ui_title(void);                     /* title screen                    */
void  ui_clear(void);
void  ui_pause(void);                     /* "Press Enter to continue"       */
void  ui_message(const char *msg);
void  ui_bar(int cur, int max, int width);/* plain bar, kept for old code    */
void  ui_show_profile(const GameState *g);
void  ui_show_tasks(const TaskList *t);
void  ui_show_boss(const Boss *b);
void  ui_show_outcome(const LogOutcome *o, int minutes);
int   ui_main_menu(const GameState *g);   /* NOTE: now takes the game state  */
void  ui_read_line(const char *prompt, char *buf, int size);
int   ui_read_int(const char *prompt, int min, int max);
Skill ui_read_skill(void);


/* ==================================================================
 * 3. THE CODE
 * ================================================================== */


/* ----- character.c ----- */
static const char *SKILL_NAMES[SKILL_COUNT] = {"Study", "Fitness", "Creativity", "Social"};
/* Three title tiers per skill: levels 1-4, 5-9, 10+ */
static const char *TITLES[SKILL_COUNT][3] = {
    {"Novice",    "Scholar",   "Sage"},
    {"Rookie",    "Athlete",   "Champion"},
    {"Doodler",   "Artisan",   "Visionary"},
    {"Wallflower","Companion", "Charmer"}
};

static int title_tier(int level) {
    int t = level / TITLE_TIER_SIZE;
    return t > 2 ? 2 : t;
}

void character_init(Character *c, const char *name) {
    memset(c, 0, sizeof *c);
    strncpy(c->name, name, MAX_NAME - 1);
}

const char *skill_name(Skill s) {
    return (s >= 0 && s < SKILL_COUNT) ? SKILL_NAMES[s] : "?";
}

/* Level L -> L+1 costs LEVEL_COST_BASE * L XP. */
int xp_to_level(int xp) {
    int level = 1, need = LEVEL_COST_BASE;
    while (xp >= need) { xp -= need; level++; need = LEVEL_COST_BASE * level; }
    return level;
}

void level_progress(int xp, int *into, int *need) {
    int level = 1, cost = LEVEL_COST_BASE;
    while (xp >= cost) { xp -= cost; level++; cost = LEVEL_COST_BASE * level; }
    *into = xp; *need = cost;
}

XpResult character_add_xp(Character *c, Skill s, int xp) {
    XpResult r;
    memset(&r, 0, sizeof r);
    r.skill = s;
    r.title = "";
    if (s < 0 || s >= SKILL_COUNT || xp <= 0) return r;
    r.level_before = xp_to_level(c->skills[s].xp);
    c->skills[s].xp += xp;
    c->total_xp     += xp;
    r.level_after    = xp_to_level(c->skills[s].xp);
    r.levels_gained  = r.level_after - r.level_before;
    r.title_unlocked = title_tier(r.level_after) > title_tier(r.level_before);
    r.title          = TITLES[s][title_tier(r.level_after)];
    return r;
}

int character_level(const Character *c) { return xp_to_level(c->total_xp); }

Skill character_best_skill(const Character *c) {
    Skill best = SKILL_STUDY;
    for (int i = 1; i < SKILL_COUNT; i++)
        if (c->skills[i].xp > c->skills[best].xp) best = (Skill)i;
    return best;
}

const char *character_title(const Character *c) {
    Skill b = character_best_skill(c);
    return TITLES[b][title_tier(xp_to_level(c->skills[b].xp))];
}

/* ----- tasks.c ----- */
void tasks_init(TaskList *t) { memset(t, 0, sizeof *t); }

int tasks_add(TaskList *t, const char *name, Skill s, int target_min) {
    if (t->count >= MAX_TASKS || target_min <= 0 || s < 0 || s >= SKILL_COUNT) return -1;
    Task *k = &t->items[t->count];
    memset(k, 0, sizeof *k);
    strncpy(k->name, name, MAX_TASK_NAME - 1);
    k->skill = s;
    k->target_min = target_min;
    return t->count++;
}

int tasks_log(TaskList *t, int index, int minutes) {
    if (index < 0 || index >= t->count || minutes <= 0) return -1;
    Task *k = &t->items[index];
    k->logged_min += minutes;
    if (k->logged_min >= k->target_min) k->done = 1;
    return 0;
}

int tasks_remove(TaskList *t, int index) {
    if (index < 0 || index >= t->count) return -1;
    for (int i = index; i < t->count - 1; i++) t->items[i] = t->items[i + 1];
    t->count--;
    return 0;
}

void tasks_reset_week(TaskList *t) {
    for (int i = 0; i < t->count; i++) { t->items[i].logged_min = 0; t->items[i].done = 0; }
}

/* ----- boss.c ----- */
typedef struct { const char *name; Skill skill; int required_min; int reward_xp; } BossTemplate;

static const BossTemplate BOSSES[] = {
    {"The Procrastination Dragon", SKILL_STUDY,    180, 150},
    {"The Couch Golem",            SKILL_FITNESS,  150, 150},
    {"The Blank Page Wraith",      SKILL_CREATIVE, 120, 120},
    {"The Awkward Silence Ogre",   SKILL_SOCIAL,   120, 120},
    {"The Exam Hydra",             SKILL_STUDY,    300, 250},
    {"The Marathon Minotaur",      SKILL_FITNESS,  240, 220}
};
#define NBOSS ((int)(sizeof BOSSES / sizeof BOSSES[0]))

/* Deterministic "random" pick from the week number: no global srand() needed,
 * so other modules' use of rand() can never change the boss. */
void boss_spawn(Boss *b, long week) {
    unsigned h = (unsigned)week * 2654435761u;
    const BossTemplate *t = &BOSSES[(h >> 16) % NBOSS];
    memset(b, 0, sizeof *b);
    strncpy(b->name, t->name, MAX_TASK_NAME - 1);
    b->skill = t->skill;
    b->required_min = t->required_min;
    b->reward_xp = t->reward_xp;
}

int boss_apply_progress(Boss *b, Skill s, int minutes) {
    if (b->defeated || s != b->skill || minutes <= 0) return 0;
    b->progress_min += minutes;
    if (b->progress_min >= b->required_min) {
        b->progress_min = b->required_min;
        b->defeated = 1;
        return 1;
    }
    return 0;
}

int boss_percent(const Boss *b) {
    return b->required_min > 0 ? b->progress_min * 100 / b->required_min : 0;
}

/* ----- storage.c ----- */
/* File format (plain text, one record per line):
 *   LIFERPG 1
 *   name <hero name>
 *   week <n>
 *   hero <total_xp> <bosses_defeated>
 *   skill <index> <xp>
 *   task <skill> <target> <logged> <done> <name...>
 *   boss <skill> <required> <progress> <reward> <defeated> <name...>
 */
int storage_save(const GameState *g, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "LIFERPG 1\nname %s\nweek %ld\nhero %d %d\n",
            g->hero.name, g->week, g->hero.total_xp, g->hero.bosses_defeated);
    for (int i = 0; i < SKILL_COUNT; i++) fprintf(f, "skill %d %d\n", i, g->hero.skills[i].xp);
    for (int i = 0; i < g->tasks.count; i++) {
        const Task *t = &g->tasks.items[i];
        fprintf(f, "task %d %d %d %d %s\n", t->skill, t->target_min, t->logged_min, t->done, t->name);
    }
    const Boss *b = &g->boss;
    fprintf(f, "boss %d %d %d %d %d %s\n", b->skill, b->required_min, b->progress_min,
            b->reward_xp, b->defeated, b->name);
    return fclose(f) == 0 ? 0 : -1;
}

static void trim_nl(char *s) { s[strcspn(s, "\r\n")] = '\0'; }

int storage_load(GameState *g, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char line[256]; int ok = 0, pos = 0, a, b, c, d, e;
    memset(g, 0, sizeof *g);
    if (!fgets(line, sizeof line, f) || strncmp(line, "LIFERPG 1", 9) != 0) { fclose(f); return -1; }
    while (fgets(line, sizeof line, f)) {
        trim_nl(line);
        if (strncmp(line, "name ", 5) == 0) { strncpy(g->hero.name, line + 5, MAX_NAME - 1); ok = 1; }
        else if (sscanf(line, "week %ld", &g->week) == 1) { }
        else if (sscanf(line, "hero %d %d", &g->hero.total_xp, &g->hero.bosses_defeated) == 2) { }
        else if (sscanf(line, "skill %d %d", &a, &b) == 2) {
            if (a >= 0 && a < SKILL_COUNT) g->hero.skills[a].xp = b;
        } else if (sscanf(line, "task %d %d %d %d %n", &a, &b, &c, &d, &pos) == 4
                   && g->tasks.count < MAX_TASKS && a >= 0 && a < SKILL_COUNT) {
            Task *t = &g->tasks.items[g->tasks.count++];
            t->skill = (Skill)a; t->target_min = b; t->logged_min = c; t->done = d;
            strncpy(t->name, line + pos, MAX_TASK_NAME - 1);
        } else if (sscanf(line, "boss %d %d %d %d %d %n", &a, &b, &c, &d, &e, &pos) == 5
                   && a >= 0 && a < SKILL_COUNT) {
            g->boss.skill = (Skill)a; g->boss.required_min = b; g->boss.progress_min = c;
            g->boss.reward_xp = d; g->boss.defeated = e;
            strncpy(g->boss.name, line + pos, MAX_TASK_NAME - 1);
        }
    }
    fclose(f);
    return ok ? 0 : -1;
}

/* ----- game.c ----- */
long game_current_week(void) { return (long)(time(NULL) / (7L * 24 * 3600)); }

void game_init(GameState *g, const char *hero_name, long week) {
    character_init(&g->hero, hero_name);
    tasks_init(&g->tasks);
    g->week = week;
    boss_spawn(&g->boss, week);
}

/* New week => clear the task log and summon a new boss. */
int game_sync_week(GameState *g, long week) {
    if (g->week == week) return 0;
    g->week = week;
    tasks_reset_week(&g->tasks);
    boss_spawn(&g->boss, week);
    return 1;
}

/* The one place where "logging effort" touches every module. */
int game_log_effort(GameState *g, int task_index, int minutes, LogOutcome *out) {
    if (tasks_log(&g->tasks, task_index, minutes) != 0) return -1;
    Skill s = g->tasks.items[task_index].skill;
    out->boss_defeated = 0;
    out->task_xp = character_add_xp(&g->hero, s, minutes * XP_PER_MINUTE);
    out->boss_defeated = boss_apply_progress(&g->boss, s, minutes);
    if (out->boss_defeated) {
        out->boss_xp = character_add_xp(&g->hero, g->boss.skill, g->boss.reward_xp);
        g->hero.bosses_defeated++;
    }
    return 0;
}

/* ----- ui.c ----- */
/* ---------- look & feel ---------- */
static int fancy = 0;   /* 1 = colours + unicode + screen clearing */

#define RST  "\x1b[0m"
#define BOLD "\x1b[1m"
#define DIM  "\x1b[2m"
static const char *COL_SKILL[SKILL_COUNT] = {"\x1b[94m", "\x1b[91m", "\x1b[95m", "\x1b[92m"};
#define CYAN   "\x1b[96m"
#define YELLOW "\x1b[93m"
#define GREEN  "\x1b[92m"
#define RED    "\x1b[91m"

static const char *C(const char *code) { return fancy ? code : ""; }
static const char *pick(const char *fancy_s, const char *plain_s) { return fancy ? fancy_s : plain_s; }

void ui_init(void) {
    fancy = IS_TTY(stdout) && getenv("NO_COLOR") == NULL;
}

void ui_clear(void) { if (fancy) fputs("\x1b[2J\x1b[H", stdout); }

void ui_pause(void) {
    char buf[8];
    printf("\n%sPress Enter to continue...%s", C(DIM), C(RST));
    fflush(stdout);
    if (!fgets(buf, sizeof buf, stdin)) exit(0);
}

void ui_message(const char *msg) { printf("\n%s>> %s%s\n", C(CYAN), msg, C(RST)); }

/* ---------- boxes ---------- */
static void rep(const char *s, int n) { while (n-- > 0) fputs(s, stdout); }

static void box_top(int w)    { fputs(pick("+", "+"), stdout); rep(pick("-", "="), w); fputs(pick("+", "+"), stdout); putchar('\n'); }
static void box_bottom(int w) { fputs(pick("+", "+"), stdout); rep(pick("-", "="), w); fputs(pick("+", "+"), stdout); putchar('\n'); }
static void box_line(const char *text, int w, const char *color) {
    int len = (int)strlen(text), left = (w - len) / 2, right = w - len - left;
    if (left < 0) { left = 0; right = 0; }
    fputs(pick("¦", "|"), stdout);
    rep(" ", left); printf("%s%s%s", C(color), text, C(RST)); rep(" ", right);
    fputs(pick("¦", "|"), stdout); putchar('\n');
}

/* ---------- bars ---------- */
static void bar_color(int cur, int max, int width, const char *color) {
    int fill = max > 0 ? cur * width / max : 0;
    if (fill > width) fill = width;
    if (fill < 0) fill = 0;
    printf("%s[%s", C(DIM), C(RST));
    fputs(C(color), stdout);
    rep(pick("¦", "#"), fill);
    fputs(C(RST), stdout); fputs(C(DIM), stdout);
    rep(pick("¦", "-"), width - fill);
    printf("%s]%s", C(RST), C(RST));
}

void ui_bar(int cur, int max, int width) { bar_color(cur, max, width, GREEN); }

/* ---------- screens ---------- */
void ui_title(void) {
    ui_clear();
    putchar('\n');
    box_top(36);
    box_line("", 36, "");
    box_line("L I F E    R P G", 36, YELLOW);
    box_line("", 36, "");
    box_line("Turn effort into experience", 36, CYAN);
    box_line("", 36, "");
    box_bottom(36);
    printf("\n   %sLog effort. Earn XP. Beat the weekly boss.%s\n", C(DIM), C(RST));
    ui_pause();
}

void ui_show_profile(const GameState *g) {
    const Character *c = &g->hero;
    printf("\n%s%s%s the %s%s%s\n", C(BOLD), c->name, C(RST), C(YELLOW), character_title(c), C(RST));
    printf("Level %d  %s|%s  %d XP  %s|%s  Bosses defeated: %d\n\n",
           character_level(c), C(DIM), C(RST), c->total_xp, C(DIM), C(RST), c->bosses_defeated);
    for (int i = 0; i < SKILL_COUNT; i++) {
        int into, need, xp = c->skills[i].xp;
        level_progress(xp, &into, &need);
        printf("%s%-11s%s Lv %-2d ", C(COL_SKILL[i]), skill_name((Skill)i), C(RST), xp_to_level(xp));
        bar_color(into, need, 20, COL_SKILL[i]);
        printf(" %d/%d XP\n", into, need);
    }
}

void ui_show_tasks(const TaskList *t) {
    printf("\n%s%s Weekly quests %s%s\n", C(BOLD), pick("--", "--"), pick("--", "--"), C(RST));
    if (t->count == 0) { printf("%s(none yet - add one!)%s\n", C(DIM), C(RST)); return; }
    for (int i = 0; i < t->count; i++) {
        const Task *k = &t->items[i];
        printf("%2d. ", i + 1);
        if (k->done) printf("%s%s%s ", C(GREEN), pick("?", "x"), C(RST)); else printf("%s ", pick("?", " "));
        printf("%-24s %s%-10s%s ", k->name, C(COL_SKILL[k->skill]), skill_name(k->skill), C(RST));
        bar_color(k->logged_min, k->target_min, 12, k->done ? GREEN : COL_SKILL[k->skill]);
        printf(" %d/%d min\n", k->logged_min, k->target_min);
    }
}

void ui_show_boss(const Boss *b) {
    int left = b->required_min - b->progress_min;
    const char *hp = left * 100 / (b->required_min ? b->required_min : 1) > 60 ? RED
                   : left * 100 / (b->required_min ? b->required_min : 1) > 25 ? YELLOW : GREEN;
    printf("\n");
    box_top(40);
    box_line("WEEKLY BOSS BATTLE", 40, YELLOW);
    box_line(b->name, 40, RED);
    box_bottom(40);
    printf("Weak to: %s%s%s effort    Reward: %d XP\n", C(COL_SKILL[b->skill]), skill_name(b->skill), C(RST), b->reward_xp);
    printf("Boss HP  "); bar_color(left, b->required_min, 24, hp); printf(" %d min left\n", left);
    if (b->defeated) printf("\n%sDEFEATED! Well fought, hero.%s\n", C(GREEN), C(RST));
}

static void show_xp(const XpResult *r) {
    char line[64];
    if (r->levels_gained > 0) {
        snprintf(line, sizeof line, "LEVEL UP!  %s  ->  Lv %d", skill_name(r->skill), r->level_after);
        box_top(34); box_line(line, 34, YELLOW); box_bottom(34);
    }
    if (r->title_unlocked) {
        snprintf(line, sizeof line, "NEW TITLE: %s", r->title);
        box_top(34); box_line(line, 34, CYAN); box_bottom(34);
    }
}

void ui_show_outcome(const LogOutcome *o, int minutes) {
    printf("\n%s+%d XP%s in %s%s%s\n", C(GREEN), minutes * XP_PER_MINUTE, C(RST),
           C(COL_SKILL[o->task_xp.skill]), skill_name(o->task_xp.skill), C(RST));
    show_xp(&o->task_xp);
    if (o->boss_defeated) {
        printf("\n%sYOU DEFEATED THE BOSS!%s\n", C(RED), C(RST));
        show_xp(&o->boss_xp);
    }
}

int ui_main_menu(const GameState *g) {
    ui_clear();
    printf("%sLIFE RPG%s  %s|%s  %s the %s%s  (Lv %d)\n", C(YELLOW), C(RST), C(DIM), C(RST),
           g->hero.name, C(YELLOW), character_title(&g->hero), character_level(&g->hero));
    printf("%s%s%s\n", C(DIM), pick("------------------------------------------------", "------------------------------------------------"), C(RST));
    printf(" %s1%s Profile    %s2%s Quests    %s3%s Add quest\n", C(CYAN), C(RST), C(CYAN), C(RST), C(CYAN), C(RST));
    printf(" %s4%s Log effort %s5%s Boss      %s6%s Save & quit\n\n", C(CYAN), C(RST), C(CYAN), C(RST), C(CYAN), C(RST));
    return ui_read_int("Choose: ", 1, 6);
}

/* ---------- input ---------- */
void ui_read_line(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fflush(stdout);
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\r\n")] = '\0';
}

int ui_read_int(const char *prompt, int min, int max) {
    char buf[64];
    for (;;) {
        ui_read_line(prompt, buf, sizeof buf);
        if (buf[0] == '\0' && feof(stdin)) exit(0);       /* input closed */
        char *end; long v = strtol(buf, &end, 10);
        if (end != buf && *end == '\0' && v >= min && v <= max) return (int)v;
        printf("%sPlease enter a number from %d to %d.%s\n", C(RED), min, max, C(RST));
    }
}

Skill ui_read_skill(void) {
    printf("Skills: ");
    for (int i = 0; i < SKILL_COUNT; i++)
        printf("%s[%d] %s%s  ", C(COL_SKILL[i]), i + 1, skill_name((Skill)i), C(RST));
    printf("\n");
    return (Skill)(ui_read_int("Skill: ", 1, SKILL_COUNT) - 1);
}

/* ==================================================================
 * 4. MAIN  (was src/main.c)
 * ================================================================== */

int main(void) {
    GameState g;
    long week = game_current_week();

    ui_init();
    ui_title();

    if (storage_load(&g, SAVE_FILE) != 0) {
        char name[MAX_NAME];
        ui_read_line("Name your hero: ", name, sizeof name);
        if (name[0] == '\0') strcpy(name, "Hero");
        game_init(&g, name, week);
    } else if (game_sync_week(&g, week)) {
        ui_message("A new week begins! Quests reset and a new boss appears.");
    }

    for (;;) {
        switch (ui_main_menu(&g)) {
        case 1: ui_show_profile(&g); break;
        case 2: ui_show_tasks(&g.tasks); break;
        case 3: {
            char name[MAX_TASK_NAME];
            ui_read_line("Quest name: ", name, sizeof name);
            Skill s = ui_read_skill();
            int target = ui_read_int("Target minutes this week (1-6000): ", 1, 6000);
            if (tasks_add(&g.tasks, name, s, target) < 0) ui_message("Quest list is full.");
            break; }
        case 4: {
            if (g.tasks.count == 0) { ui_message("Add a quest first."); break; }
            ui_show_tasks(&g.tasks);
            int i = ui_read_int("Which quest? ", 1, g.tasks.count) - 1;
            int m = ui_read_int("Minutes of effort (1-1440): ", 1, 1440);
            LogOutcome out;
            if (game_log_effort(&g, i, m, &out) == 0) ui_show_outcome(&out, m);
            break; }
        case 5: ui_show_boss(&g.boss); break;
        case 6:
            if (storage_save(&g, SAVE_FILE) != 0) ui_message("Could not save!");
            else ui_message("Progress saved. See you next time, hero!");
            return 0;
        }
        ui_pause();
    }
}
