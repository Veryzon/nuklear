/* nuklear - 1.32.0 - public domain */
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <time.h>

#include "qwadro/inc/afxQwadro.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_4D_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "../../nuklear.h"
#include "nuklear_qwadro.h"

// based on GLFW3 GL3 example

/* ===============================================================
 *
 *                          EXAMPLE
 *
 * ===============================================================*/
/* This are some code examples to provide a small overview of what can be
 * done with this library. To try out an example uncomment the defines */
#if !0
#define INCLUDE_ALL
#define INCLUDE_STYLE
#define INCLUDE_CALCULATOR
#define INCLUDE_CANVAS
#define INCLUDE_OVERVIEW
#define INCLUDE_CONFIGURATOR
#define INCLUDE_NODE_EDITOR
#endif

#ifdef INCLUDE_ALL
  #define INCLUDE_STYLE
  #define INCLUDE_CALCULATOR
  #define INCLUDE_CANVAS
  #define INCLUDE_OVERVIEW
  #define INCLUDE_CONFIGURATOR
  #define INCLUDE_NODE_EDITOR
#endif

#ifdef INCLUDE_STYLE
  #include "../../demo/common/style.c"
#endif
#ifdef INCLUDE_CALCULATOR
  #include "../../demo/common/calculator.c"
#endif
#ifdef INCLUDE_CANVAS
  #include "../../demo/common/canvas.c"
#endif
#ifdef INCLUDE_OVERVIEW
  #include "../../demo/common/overview.c"
#endif
#ifdef INCLUDE_CONFIGURATOR
  #include "../../demo/common/style_configurator.c"
#endif
#ifdef INCLUDE_NODE_EDITOR
  #include "../../demo/common/node_editor.c"
#endif

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
static void error_callback(int e, const char *d)
{printf("Error %d: %s\n", e, d);}

int main(void)
{
    afxError err;
    // Boot up the Qwadro (if necessary)

    afxSystemConfig sysc = { 0 };
    AfxConfigureSystem(&sysc);
    AfxBootstrapSystem(&sysc);

    // Set up the draw system

    afxUnit drawIcd = 0;
    afxDrawSystem dsys;
    afxDrawSystemConfig dsyc = { 0 };
    AvxConfigureDrawSystem(drawIcd, &dsyc);
    dsyc.exuCnt = 1;
    AvxEstablishDrawSystem(drawIcd, &dsyc, &dsys);
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    // Open a session

    afxUnit shIcd = 0;
    afxSession ses;
    afxSessionConfig scfg = { 0 };
    scfg.dsys = dsys; // integrate our draw system
    //scfg.msys = msys; // integrate our mix system
    AfxAcquireSession(shIcd, &scfg, &ses);
    AFX_ASSERT_OBJECTS(afxFcc_SES, 1, &ses);
    AfxOpenSession(ses, NIL, NIL, NIL);


    /* Platform */
    struct nk_afx afx = { 0 };
    afxWindow win;
    int width = 0, height = 0;
    struct nk_context *ctx;
    struct nk_colorf bg;


    // Acquire a drawable surface

    afxWindow wnd;
    afxWindowConfig wrc = { 0 };
    wrc.udd = &afx;
    wrc.dsys = dsys;
    AfxConfigureWindow(&wrc, NIL, AFX_V3D(0.5, 0.5, 1));
    //wrc.surface.bufFmt[0] = avxFormat_BGRA4un;
    //wrc.surface.bufFmt[1] = avxFormat_D32fS8u;
    wrc.surface.bufFmt[1] = avxFormat_D32f;
    //wrc.surface.bufFmt[2] = avxFormat_S8u;
    AfxAcquireWindow(&wrc, &wnd);
    win = wnd;
    //AfxAdjustWindowFromNdc(wnd, NIL, AFX_V3D(0.5, 0.5, 1));

    afxDrawOutput dout;
    AfxGetWindowDrawOutput(wnd, FALSE, &dout);
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);

    avxFence fence;
    AvxAcquireFences(dsys, FALSE, 1, &fence);

    #ifdef INCLUDE_CONFIGURATOR
    static struct nk_color color_table[NK_COLOR_COUNT];
    memcpy(color_table, nk_default_color_style, sizeof(color_table));
    #endif

    afxRect rc;
    AfxGetWindowRect(win, &rc, NIL);
    width = rc.w;
    height = rc.h;

    ctx = nk_afx_init(&afx, win, NK_AFX_INSTALL_CALLBACKS);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
    nk_afx_font_stash_begin(&afx, &atlas);
    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
    nk_afx_font_stash_end(&afx);
    /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
    /*nk_style_set_font(ctx, &droid->handle);*/}

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (1)
    {
        /* Input */
        nk_afx_new_frame(&afx);

        /* GUI */
        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        /* -------------- EXAMPLES ---------------- */
        #ifdef INCLUDE_CALCULATOR
          calculator(ctx);
        #endif
        #ifdef INCLUDE_CANVAS
          canvas(ctx);
        #endif
        #ifdef INCLUDE_OVERVIEW
          overview(ctx);
        #endif
        #ifdef INCLUDE_CONFIGURATOR
          style_configurator(ctx, color_table);
        #endif
        #ifdef INCLUDE_NODE_EDITOR
          node_editor(ctx);
        #endif
        /* ----------------------------------------- */

        /* Draw */
#if 0
        afxGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        /* IMPORTANT: `nk_afx_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
        nk_afx_render(&afx, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        afxSwapBuffers(win);
#endif
        afxUnit outBufIdx = 0;
        if (AvxRequestDrawOutputBuffer(dout, 0, &outBufIdx))
            continue;

        afxUnit portId = 0;
        afxUnit queIdx = 0;
        portId = 0;
        queIdx = 0;
        afxDrawContext dctx;
        if (AvxAcquireDrawContexts(dsys, portId, queIdx, 1, &dctx))
        {
            AfxThrowError();
            AvxRecycleDrawOutputBuffer(dout, outBufIdx);
            continue;
        }

        avxCanvas canv;
        avxRange canvWhd;
        AvxGetDrawOutputCanvas(dout, outBufIdx, &canv);
        AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);
        canvWhd = AvxGetCanvasArea(canv, AVX_ORIGIN_ZERO);

        {
            avxDrawScope dps = { 0 };
            dps.canv = canv;
            dps.area = AVX_RECT(0, 0, canvWhd.w, canvWhd.h);
            dps.layerCnt = 1;
            dps.targetCnt = 1;
            dps.targets[0].clearValue.rgba[0] = bg.r;
            dps.targets[0].clearValue.rgba[1] = bg.g;
            dps.targets[0].clearValue.rgba[2] = bg.b;
            dps.targets[0].clearValue.rgba[3] = bg.a;
            dps.targets[0].loadOp = avxLoadOp_CLEAR;
            dps.targets[0].storeOp = avxStoreOp_STORE;
            dps.depth.clearValue.depth = 1.0;
            dps.depth.clearValue.stencil = 0;
            dps.depth.loadOp = avxLoadOp_CLEAR;
            //dps.depth.storeOp = avxStoreOp_STORE;            

            AvxCmdCommenceDrawScope(dctx, &dps);

            nk_afx_render(&afx, NK_ANTI_ALIASING_OFF, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER, dctx);

            //TestSvg(vg, dctx, canvWhd);
            AvxCmdConcludeDrawScope(dctx);
        }

        afxSemaphore dscrCompleteSem = NIL;

        if (AvxCompileDrawCommands(dctx))
        {
            AfxThrowError();
            AvxRecycleDrawOutputBuffer(dout, outBufIdx);
            AfxDisposeObjects(1, &dctx);
            continue;
        }

        avxSubmission subm = { 0 };
        avxFence drawCompletedFence = fence;
        AvxResetFences(dsys, 1, &fence);

        if (AvxExecuteDrawCommands(dsys, &subm, 1, &dctx, drawCompletedFence))
        {
            AfxThrowError();
            AvxRecycleDrawOutputBuffer(dout, outBufIdx);
            AfxDisposeObjects(1, &dctx);
            continue;
        }
        AfxDisposeObjects(1, &dctx);

        //AvxWaitForDrawQueue(dsys, subm.exuIdx, subm.baseQueIdx, 0);
        //AvxWaitForDrawSystem(dsys, 1);

        AFX_ASSERT(3 > outBufIdx);
        avxPresentation pres = { 0 };
        if (AvxPresentDrawOutputs(dsys, &pres, NIL, 1, &dout, &outBufIdx, NIL))
        {
            AfxThrowError();
            AvxRecycleDrawOutputBuffer(dout, outBufIdx);
            continue;
        }

        AfxPollInput(NIL, 0);
    }
    nk_afx_shutdown(&afx);

    AfxDisposeObjects(1, &wnd);
    AfxDisposeObjects(1, &ses);
    AfxDisposeObjects(1, &dsys);

    AfxDoSystemShutdown(0);
    AfxYield();

    return 0;
}

