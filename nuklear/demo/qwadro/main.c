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

#include "qwadro/afxQwadro.h"

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
#include "nuklear_qwadro.h"

// based on GLFW3 GL3 example

#if 0

afxCmdId AfxCmdTextInputInlined(struct nk_context *ctx, afxTextInputInfo const* info)
{
    if (pushCols)
    {
        nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 1);
    }
    if (pushRows)
    {
        nk_layout_row_push(ctx, 0.333); // relative width
    }
    //nk_layout_row_static(ctx, 25, 200, 1);
    nk_edit_string(ctx, NK_EDIT_FIELD, info->caption.start, &info->caption.len, info->caption.cap, nk_filter_default);
    if (pushCols)
    {
        nk_layout_row_end(ctx);
    }
    return 0;
}

AFX_DEFINE_STRUCT(afxCheckboxInfo)
{
    afxString caption;
    afxString hint; // on_hover_text
};

afxCmdId AfxCmdCheckbox(struct nk_context *ctx, afxCheckboxInfo const* info, afxBool* checked)
{
    if (pushCols)
    {
        nk_layout_row_begin(ctx, NK_DYNAMIC, 30, 1);
    }
    if (pushRows)
    {
        nk_layout_row_push(ctx, 0.333); // relative width
    }
    nk_checkbox_text(ctx, info->caption.start, info->caption.len, checked);
    if (pushCols)
    {
        nk_layout_row_end(ctx);
    }
    return 0;
}

afxCmdId AfxCmdDisable(struct nk_context *ctx)
{

}

afxCmdId AfxCmdSeparator(struct nk_context *ctx)
{
    
}
#endif

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
    AfxConfigureSystem(&sysc, NIL);
    AfxBootstrapSystem(&sysc);

    // Set up the draw system

    afxUnit drawIcd = 0;
    afxDrawSystem dsys;
    avxSystemConfig dsyc = { 0 };
    dsyc.caps = avxAptitude_GFX;
    dsyc.accel = afxAcceleration_DPU;
    dsyc.exuCnt = 1;
    AvxConfigureDrawSystem(drawIcd, &dsyc);
    AvxEstablishDrawSystem(drawIcd, &dsyc, &dsys);
    AFX_ASSERT_OBJECTS(afxFcc_DSYS, 1, &dsys);

    // Open a session

    afxUnit shIcd = 0;
    afxEnvironment env;
    afxEnvironmentConfig ecfg = { 0 };
    ecfg.dsys = dsys; // integrate our draw system
    //scfg.msys = msys; // integrate our mix system
    AfxConfigureEnvironment(shIcd, &ecfg);
    AfxEstablishEnvironment(shIcd, &ecfg, &env);
    AFX_ASSERT_OBJECTS(afxFcc_ENV, 1, &env);


    /* Platform */
    struct nk_afx afx = { 0 };
    afxWindow win;
    int width = 0, height = 0;
    struct nk_context *ctx;
    struct nk_colorf bg;


    // Acquire a drawable surface

    afxWindow wnd;
    afxSurface dout;
    afxWindowConfig wcfg = { 0 };
    wcfg.udd = &afx;
    wcfg.dout.dsys = dsys;
    //wcfg.dout.ccfg.bins[0].fmt = avxFormat_BGRA4un;
    //wcfg.dout.ccfg.bins[1].fmt = avxFormat_D32fS8u;
    wcfg.anchor = afxAnchor_CENTER | afxAnchor_TOP;
    AfxConfigureWindow(env, &wcfg, NIL, AFX_V3D(0.5, 0.5, 1));
    wcfg.dout.ccfg.binCnt += 1;
    wcfg.dout.ccfg.bins[1].fmt = avxFormat_D32f;
    //wcfg.dout.ccfg.bins[2].fmt = avxFormat_S8u;
    AfxAcquireWindow(env, &wcfg, &wnd);
    AFX_ASSERT_OBJECTS(afxFcc_WND, 1, &wnd);
    AfxGetWindowSurface(wnd, &dout);
    AFX_ASSERT_OBJECTS(afxFcc_DOUT, 1, &dout);

    win = wnd;

    avxFence fence;
    avxFenceInfo fenci = { 0 };
    fenci.initialVal = FALSE;
    AvxAcquireFences(dsys, 1, &fenci, &fence);

    #ifdef INCLUDE_CONFIGURATOR
    static struct nk_color color_table[NK_COLOR_COUNT];
    memcpy(color_table, nk_default_color_style, sizeof(color_table));
    #endif

    afxRect rc;
    AfxGetWindowRect(win, afxAnchor_CENTER | afxAnchor_MIDDLE, &rc);
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

#if 0
    afxDrawQueue drawQueues[3];
    if (AvxAcquireDrawQueues(dsys, 0, 3, NIL, drawQueues))
    {
        AfxThrowError();
    }
#endif

    afxDrawContext drawContexts[3];
    avxContextConfig dctxi = { 0 };
    dctxi.caps = avxAptitude_GFX;
    AvxAcquireDrawContexts(dsys, NIL, &dctxi, 3, drawContexts);

    afxWidget wid;
    afxWidgetConfig widc = { 0 };
    AfxAcquireWidgets(wnd, 1, &widc, &wid);

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

    afxClock startClock, lastClock;
    AfxGetClock(&startClock);
    lastClock = startClock;
    afxReal64 ft = 0;
    afxUnit fpsi = 0;
    afxUnit fps = 0;

    while (1)
    {
        /* Input */
        nk_afx_new_frame(&afx);
#if 0
        {
            struct nk_color windows7_bg = nk_rgba(240, 240, 240, 255);  // Light gray background
            struct nk_color windows7_fg = nk_rgba(0, 0, 0, 255);        // Black text
            struct nk_color windows7_button = nk_rgba(70, 130, 180, 255); // Blue button color
            struct nk_color windows7_button_hover = nk_rgba(100, 150, 210, 255); // Hover state

            // Set up Nuklear theme
            struct nk_style *style = &ctx->style;
            style->window.background = windows7_bg;
            style->button.normal = nk_style_item_color(windows7_button);
            style->button.hover = nk_style_item_color(windows7_button_hover);
            style->button.active = nk_style_item_color(nk_rgba(100, 100, 255, 255)); // Active button color
            style->text.color = windows7_fg;
            style->window.header.normal = nk_style_item_color(nk_rgba(0, 120, 215, 255)); // Blue header
            style->window.header.hover = nk_style_item_color(nk_rgba(30, 144, 255, 255));
            style->window.header.active = nk_style_item_color(nk_rgba(100, 149, 237, 255));

            style->window.rounding = AFX_PI;
            style->button.rounding = AFX_PI;
            style->slider.rounding = AFX_PI;
            style->window.border_color = nk_rgba(200, 200, 200, 255); // Light gray border
            //style->window.border_width = 2;  // Thin border
            style->window.border = 2;  // Thin border
            style->window.header.label_normal = (nk_rgba(0, 120, 215, 255)); // Blue window title
            style->window.header.label_hover = (nk_rgba(70, 130, 180, 255)); // Hover effect
            style->window.header.label_active = (nk_rgba(100, 149, 237, 255)); // Active window title

            style->button.normal = nk_style_item_color(nk_rgba(0, 120, 215, 255)); // Normal state
            style->button.hover = nk_style_item_color(nk_rgba(0, 130, 255, 255));  // Hover state
            style->button.active = nk_style_item_color(nk_rgba(70, 130, 180, 255)); // Active state

            style->menu_button.normal = nk_style_item_color(nk_rgba(240, 240, 240, 255));
            style->menu_button.hover = nk_style_item_color(nk_rgba(100, 149, 237, 255)); // Hover effect
            style->menu_button.active = nk_style_item_color(nk_rgba(70, 130, 180, 255)); // Active state

            style->window.background = nk_rgba(255, 255, 255, 150); // Semi-transparent white for the "glass" effect
            style->window.border_color = nk_rgba(200, 200, 200, 200); // Soft light border
        }
#endif

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
#if 0
        if (afxError_SUCCESS == AfxLockWidget(wid))
        {
            afxString2048 sb;
            AfxMakeString2048(&sb, &AFX_STRING("# Windows Options"));

            afxPanelInfo panelInfo = { 0 };
            panelInfo.bounds = AFX_RECT(50, 50, 500, 500);
            panelInfo.caption = sb.s;
            if (AfxGuiCommencePanel(wid, &panelInfo))
            {
                if (AfxGuiPushLayout(wid, auxLayoutDirection_HORIZONTAL))
                {
                    afxLabelInfo labelInfo = { 0 };
                    labelInfo.caption = AFX_STRING("title:");
                    AfxGuiLabel(wid, &labelInfo);
                    afxTextInputInfo teditInfo = { 0 };
                    teditInfo.caption = sb.s;
                    AfxGuiTextInputInlined(wid, &teditInfo);
                    AfxGuiPopLayout(wid);
                }
                if (AfxGuiPushLayout(wid, auxLayoutDirection_HORIZONTAL))
                {
                    if (AfxGuiPushGroup(wid, &AFX_STRING("group")))
                    {
                        if (AfxGuiPushLayout(wid, auxLayoutDirection_VERTICAL))
                        {
                            afxBool checked = TRUE;
                            afxCheckboxInfo chkInfo = { 0 };
                            chkInfo.caption = AFX_STRING("1");
                            AfxGuiCheckbox(wid, &chkInfo, &checked);
                            chkInfo.caption = AFX_STRING("2");
                            AfxGuiCheckbox(wid, &chkInfo, &checked);
                            chkInfo.caption = AFX_STRING("3");
                            AfxGuiCheckbox(wid, &chkInfo, &checked);
                            AfxGuiPopLayout(wid);
                        }
                        AfxGuiPopGroup(wid);
                    }
                    if (AfxGuiPushGroup(wid, &AFX_STRING("group1")))
                    {
                        if (AfxGuiPushLayout(wid, auxLayoutDirection_VERTICAL))
                        {
                            afxBool checked = TRUE;
                            afxCheckboxInfo chkInfo = { 0 };
                            chkInfo.caption = AFX_STRING("anchored");
                            AfxGuiCheckbox(wid, &chkInfo, &checked);

                            AfxGuiPopLayout(wid);
                        }
                        AfxGuiPopGroup(wid);
                    }
                    AfxGuiPopLayout(wid);
                }
                AfxGuiConcludePanel(wid);
            }
            AfxUnlockWidget(wid);
        }
#endif
#if !0
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
#endif

        afxUnit outBufIdx = 0;
        if (AfxFailed(AvxLockSurfaceBuffer(dout, AFX_TIMEOUT_IGNORED, NIL, NIL, &outBufIdx)))
            continue;

#if 0
        afxDrawContext dctx = drawContexts[outBufIdx];
#else
        afxDrawContext dctx;
        avxContextConfig dcc = { 0 };
        dcc.caps = avxAptitude_GFX;
        AvxAcquireDrawContexts(dsys, NIL, &dcc, 1, &dctx);
#endif
        if (AfxFailed(AvxPrepareDrawCommands(dctx, TRUE, avxCmdFlag_ONCE)))
        {
            AfxThrowError();
            AvxUnlockSurfaceBuffer(dout, outBufIdx);
            continue;
        }
        else
        {
#if 0
            AfxReportf(0, AfxHere(), "%d %d", outBufIdx, frameBatchId);

            if (frameBatchId != 0)
            {
                int a = 1;
            }
#endif
            avxCanvas canv;
            afxLayeredRect bounds;
            AvxGetSurfaceCanvas(dout, outBufIdx, &canv, &bounds);
            AFX_ASSERT_OBJECTS(afxFcc_CANV, 1, &canv);

            avxDrawScope dps = { 0 };
            dps.canv = canv;
            dps.bounds = bounds;
            dps.targetCnt = 1;
            dps.targets[0].clearVal.rgba[0] = bg.r;
            dps.targets[0].clearVal.rgba[1] = bg.g;
            dps.targets[0].clearVal.rgba[2] = bg.b;
            dps.targets[0].clearVal.rgba[3] = bg.a;
            dps.targets[0].loadOp = avxLoadOp_CLEAR;
            dps.targets[0].storeOp = avxStoreOp_STORE;
            dps.ds[0].clearVal.depth = 1.0;
            dps.ds[0].clearVal.stencil = 0;
            dps.ds[0].loadOp = avxLoadOp_CLEAR;
            //dps.ds[0].storeOp = avxStoreOp_STORE;            

            if (AfxSucceded(AvxCmdCommenceDrawScope(dctx, &dps)))
            {

                nk_afx_render(&afx, NK_ANTI_ALIASING_OFF, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER, dctx);

                //AfxRedrawWidgets(wnd, &dps.bounds.area, dctx);

                //TestSvg(vg, dctx, canvWhd);
                AvxCmdConcludeDrawScope(dctx);
            }

            afxSemaphore dscrCompleteSem = NIL;

            if (AfxFailed(AvxCompileDrawCommands(dctx)))
            {
                AfxThrowError();
                AvxUnlockSurfaceBuffer(dout, outBufIdx);
                AvxExhaustDrawContext(dctx, TRUE);
                continue;
            }

            avxSubmission subm = { 0 };
            subm.exuMask = AFX_BITMASK(0);
            avxFence drawCompletedFence = fence;
            //AvxResetFences(dsys, 1, &fence);
            //subm.signal = drawCompletedFence;
            subm.dctx = dctx;

            //if (AvxExecuteDrawCommands(dque, &subm, 1, &dctx))
            if (AfxFailed(AvxExecuteDrawCommands(dsys, 1, &subm, NIL)))
            {
                AfxThrowError();
                AvxUnlockSurfaceBuffer(dout, outBufIdx);
                AvxExhaustDrawContext(dctx, TRUE);
                continue;
            }
        }

        AfxDisposeObjects(1, &dctx);

        //AvxWaitForDrawQueue(dsys, 0, subm.baseQueIdx, 0);
        //AvxWaitForDrawBridges(dsys, AFX_TIMEOUT_INFINITE, subm.exuMask);
        //AvxWaitForDrawSystem(dsys, AFX_TIMEOUT_INFINITE);
#if 0
        int a = 0;
        while (AvxDoesDrawCommandsExist(dctx, 0))
            AvxRecycleDrawCommands(dctx, 0, FALSE),
            AvxWaitForDrawBridges(dsys, subm.exuMask, AFX_TIMEOUT_INFINITE),
            ++a,
            AfxReportf(0, AfxHere(), "a %d", a);

        if (frameBatchId!=0)
        //if (AvxDoesDrawCommandsExist(dctx, 0))
        {
            AfxReportf(0, AfxHere(), "nz %d", frameBatchId);
        }
        //AFX_ASSERT(!AvxDoesDrawCommandsExist(dctx, frameBatchId));
#endif
        AFX_ASSERT(3 > outBufIdx);
        avxPresentation pres = { 0 };
        pres.dout = dout;
        pres.bufIdx = outBufIdx;

        if (AfxFailed(AvxPresentSurfaces(dsys, 1, &pres, NIL)))
        {
            AfxThrowError();
            AvxUnlockSurfaceBuffer(dout, outBufIdx);
            continue;
        }

        afxClock currClock;
        AfxGetClock(&currClock);
        afxReal64 ct = AfxGetSecondsElapsed(&startClock, &currClock);
        afxReal64 dt = AfxGetSecondsElapsed(&lastClock, &currClock);
        lastClock = currClock;

        if (ct - ft >= 1.0)
        {
            fps = fpsi;
            fpsi = 0;
            ft = ct;
        }
        ++fpsi;
        AfxFormatWindowTitle(wnd, "FPS %u %u", fps, 0);


        AfxDoUx(NIL, AFX_TIMEOUT_INFINITE);
    }
    nk_afx_shutdown(&afx);

    AfxDisposeObjects(1, &wnd);
    AfxDisposeObjects(1, &env);
    AfxDisposeObjects(1, &dsys);

    AfxAbolishSystem(0);
    AfxYield();

    return 0;
}

