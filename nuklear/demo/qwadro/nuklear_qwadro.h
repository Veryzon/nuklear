/*
 * Nuklear - 1.32.0 - public domain
 * no warrenty implied; use at your own risk.
 * authored from 2015-2016 by Micha Mettke
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#ifndef NK_AFX_H_
#define NK_AFX_H_

// based on GLFW3 GL3 example

#include "qwadro/inc/afxQwadro.h"

enum nk_afx_init_state{
    NK_AFX_DEFAULT=0,
    NK_AFX_INSTALL_CALLBACKS
};

#ifndef NK_AFX_TEXT_MAX
#define NK_AFX_TEXT_MAX 256
#endif

struct nk_afx_device
{
    afxDrawSystem dsys;
    struct nk_buffer cmds;
    struct nk_draw_null_texture tex_null;
    avxBuffer vbo, ebo;
    afxSize vboSiz, iboSiz;
    void* mappedVbo,* mappedIbo;
    avxVertexInput vao;
    avxPipeline pip;
    avxRaster font_tex;
    avxSampler font_samp;
};

struct nk_afx
{
    afxWindow win;
    int width, height;
    int display_width, display_height;
    struct nk_afx_device ogl;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
    struct nk_vec2 fb_scale;
    unsigned int text[NK_AFX_TEXT_MAX];
    int text_len;
    struct nk_vec2 scroll;
    double last_button_click;
    int is_double_click_down;
    struct nk_vec2 double_click_pos;
    float delta_time_seconds_last;
};

NK_API struct nk_context*   nk_afx_init(struct nk_afx* afx, afxWindow win, enum nk_afx_init_state);
NK_API void                 nk_afx_shutdown(struct nk_afx* afx);
NK_API void                 nk_afx_font_stash_begin(struct nk_afx* afx, struct nk_font_atlas **atlas);
NK_API void                 nk_afx_font_stash_end(struct nk_afx* afx);
NK_API void                 nk_afx_new_frame(struct nk_afx* afx);
NK_API void                 nk_afx_render(struct nk_afx* afx, enum nk_anti_aliasing, int max_vertex_buffer, int max_element_buffer, afxDrawContext dctx);

NK_API void                 nk_afx_device_destroy(struct nk_afx* afx);
NK_API void                 nk_afx_device_create(struct nk_afx* afx);

NK_API void                 nk_afx_char_callback(afxWindow win, unsigned int codepoint);
NK_API void                 nk_afx_scroll_callback(afxWindow win, double xoff, double yoff);
NK_API void                 nk_afx_mouse_button_callback(afxWindow win, int button, int action, int mods);

#endif
/*
 * ==============================================================
 *
 *                          IMPLEMENTATION
 *
 * ===============================================================
 */
#ifdef NK_4D_IMPLEMENTATION
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifndef NK_AFX_DOUBLE_CLICK_LO
#define NK_AFX_DOUBLE_CLICK_LO 0.02
#endif
#ifndef NK_AFX_DOUBLE_CLICK_HI
#define NK_AFX_DOUBLE_CLICK_HI 0.2
#endif

struct nk_afx_vertex {
    float position[2];
    float uv[2];
    nk_byte col[4];
};

#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 300 es\n"
#endif

NK_API void
nk_afx_device_create(struct nk_afx* afx)
{
    struct nk_afx_device *dev = &afx->ogl;
    nk_buffer_init_default(&dev->cmds);

#if 0
    GLint status;
    static const GLchar *vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const GLchar *fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    dev->prog = glCreateProgram();
    dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
    dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
    glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
    glCompileShader(dev->vert_shdr);
    glCompileShader(dev->frag_shdr);
    glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
    assert(status == GL_TRUE);
    glAttachShader(dev->prog, dev->vert_shdr);
    glAttachShader(dev->prog, dev->frag_shdr);
    glLinkProgram(dev->prog);
    glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
    assert(status == GL_TRUE);

    dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
    dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
    dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
    dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
    dev->attrib_col = glGetAttribLocation(dev->prog, "Color");
#endif

    static const afxChar *vertex_shader =
        "\nPUSH(Mtx)\n"
        "{\n"
        "   mat4 ProjMtx;\n"
        "};\n"
        "IN(0, vec2, Position);\n"
        "IN(1, vec2, TexCoord);\n"
        "IN(2, vec4, Color);\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";
    static const afxChar *fragment_shader =
        "\nprecision mediump float;\n"
        "TEXTURE(0, 0, sampler2D, Texture);\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "OUT(0, vec4, Out_Color);\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    avxShader vsh, fsh;
    afxString vcode, fcode;
    AfxMakeString(&vcode, 0, vertex_shader, 0);
    AfxMakeString(&fcode, 0, fragment_shader, 0);
    AvxAcquireShaders(dev->dsys, 1, NIL, &vcode, &vsh);
    AvxAcquireShaders(dev->dsys, 1, NIL, &fcode, &fsh);

    avxVertexLayout vlay = { 0 };
    vlay.attrCnt = 3;
    vlay.attrs[0].fmt = avxFormat_RG32f;
    vlay.attrs[0].location = 0;
    vlay.attrs[0].offset = offsetof(struct nk_afx_vertex, position);
    vlay.attrs[0].srcIdx = 0;
    vlay.attrs[1].fmt = avxFormat_RG32f;
    vlay.attrs[1].location = 1;
    vlay.attrs[1].offset = offsetof(struct nk_afx_vertex, uv);
    vlay.attrs[1].srcIdx = 0;
    vlay.attrs[2].fmt = avxFormat_RGBA8un;
    vlay.attrs[2].location = 2;
    vlay.attrs[2].offset = offsetof(struct nk_afx_vertex, col);
    vlay.attrs[2].srcIdx = 0;
    vlay.srcCnt = 1;
    vlay.srcs[0].instanceRate = 0;
    vlay.srcs[0].srcIdx = 0;
    avxVertexInput vdecl;
    AvxDeclareVertexInputs(dev->dsys, 1, &vlay, &vdecl);

#if 0
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);
#endif
    avxPipeline pip;
    avxPipelineBlueprint pipb = { 0 };
    pipb.primTop = avxTopology_TRI_LIST;
    pipb.cullMode = avxCullMode_NONE;
    pipb.fillMode = avxFillMode_FACE;
    //pipb.depthTestEnabled = TRUE;
    //pipb.depthCompareOp = avxCompareOp_ALWAYS;
    //pipb.depthWriteDisabled = FALSE;
    pipb.vpCnt = 1;
    pipb.colorOutCnt = 1;
    pipb.colorOuts[0].blendEnabled = TRUE;
    pipb.colorOuts[0].blendConfig.rgbSrcFactor = avxBlendFactor_SRC_A;
    pipb.colorOuts[0].blendConfig.rgbDstFactor = avxBlendFactor_ONE_MINUS_SRC_A;
    pipb.colorOuts[0].blendConfig.rgbBlendOp = avxBlendOp_ADD;
    pipb.colorOuts[0].blendConfig.aSrcFactor = avxBlendFactor_SRC_A;
    pipb.colorOuts[0].blendConfig.aDstFactor = avxBlendFactor_ONE;
    pipb.colorOuts[0].blendConfig.aBlendOp = avxBlendOp_ADD;
    pipb.vin = vdecl;
    AvxAssemblePipelines(dev->dsys, 1, &pipb, &pip);
    AvxRelinkPipelineFunction(pip, avxShaderType_VERTEX, vsh, NIL, NIL, NIL);
    AvxRelinkPipelineFunction(pip, avxShaderType_FRAGMENT, fsh, NIL, NIL, NIL);
    dev->pip = pip;

    AfxDisposeObjects(1, &vsh);
    AfxDisposeObjects(1, &fsh);
    AfxDisposeObjects(1, &vdecl);

    {
        /* buffer setup */
        afxSize vs = sizeof(struct nk_afx_vertex);
        size_t vp = offsetof(struct nk_afx_vertex, position);
        size_t vt = offsetof(struct nk_afx_vertex, uv);
        size_t vc = offsetof(struct nk_afx_vertex, col);

#if 0
        glGenBuffers(1, &dev->vbo);
        glGenBuffers(1, &dev->ebo);
        glGenVertexArrays(1, &dev->vao);

        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glEnableVertexAttribArray((GLuint)dev->attrib_pos);
        glEnableVertexAttribArray((GLuint)dev->attrib_uv);
        glEnableVertexAttribArray((GLuint)dev->attrib_col);

        glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
        glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
        glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
#endif

        avxBuffer buffers[2] = { 0 };
        avxBufferInfo infos[2] = { 0 };
        infos[0].usage = avxBufferUsage_VERTEX;
        infos[0].size = dev->vboSiz;
        infos[0].flags = avxBufferFlag_WX;
        infos[1].usage = avxBufferUsage_INDEX;
        infos[1].size = dev->iboSiz;
        infos[1].flags = avxBufferFlag_WX;
        AvxAcquireBuffers(dev->dsys, 2, infos, buffers);
        dev->vbo = buffers[0];
        dev->ebo = buffers[1];

        avxBufferedMap maps[2] = { 0 };
        maps[0].buf = buffers[0];
        maps[0].range = infos[0].size;
        maps[1].buf = buffers[1];
        maps[1].range = infos[1].size;
        AvxMapBuffers(dev->dsys, 2, maps, (void**[]) { &dev->mappedVbo, &dev->mappedIbo });
    }
#if 0
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
#endif
}

NK_INTERN void
nk_afx_device_upload_atlas(struct nk_afx* afx, const void *image, int width, int height)
{
    struct nk_afx_device *dev = &afx->ogl;
#if 0
    glGenTextures(1, &dev->font_tex);
    glBindTexture(GL_TEXTURE_2D, dev->font_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image);
#endif
    avxRaster ras;
    avxRasterInfo rasi = { 0 };
    rasi.usage = avxRasterUsage_RESAMPLE;
    rasi.fmt = avxFormat_RGBA8un;
    rasi.extent.w = width;
    rasi.extent.h = height;
    AvxAcquireRasters(dev->dsys, 1, &rasi, &ras);

    if (image)
    {
        avxRasterIo iop = { 0 };
        iop.rgn.extent.w = rasi.extent.w;
        iop.rgn.extent.h = rasi.extent.h;
        iop.rgn.extent.d = 1;
        AvxUpdateRaster(ras, image, 1, &iop, 0);
        AvxWaitForDrawBridges(dev->dsys, 0, 0);
    }
    dev->font_tex = ras;

    avxSampler samp;
    avxSamplerInfo scfg = { 0 };
    scfg.magnify = avxTexelFilter_LINEAR;
    scfg.minify = avxTexelFilter_LINEAR;
    AvxDeclareSamplers(dev->dsys, 1, &scfg, &samp);
    dev->font_samp = samp;
}

NK_API void
nk_afx_device_destroy(struct nk_afx* afx)
{
    struct nk_afx_device *dev = &afx->ogl;

    AfxDisposeObjects(1, &dev->pip);
    AfxDisposeObjects(1, &dev->font_tex);
    AfxDisposeObjects(1, &dev->font_samp);
    AfxDisposeObjects(1, &dev->vbo);
    AfxDisposeObjects(1, &dev->ebo);
    nk_buffer_free(&dev->cmds);
}

NK_API void
nk_afx_render(struct nk_afx* afx, enum nk_anti_aliasing AA, int max_vertex_buffer, int max_element_buffer, afxDrawContext dctx)
{
    struct nk_afx_device *dev = &afx->ogl;
    struct nk_buffer vbuf, ebuf;

    afxV2d scale = { (afxReal)afx->display_width / (afxReal)afx->width, (afxReal)afx->display_height / (afxReal)afx->height }; // cW / wW --- canvas / window

    afxReal ortho[4][4] = {
        {2.0f, 0.0f, 0.0f, 0.0f},
        {0.0f,-2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f,-1.0f, 0.0f},
        {-1.0f,1.0f, 0.0f, 1.0f},
    };
    ortho[0][0] /= (afxReal)afx->width;
    ortho[1][1] /= (afxReal)afx->height;

#if 0
    /* setup global state */
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    /* setup program */
    glUseProgram(dev->prog);
    glUniform1i(dev->uniform_tex, 0);
    glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
    glViewport(0, 0, (GLsizei)afx->display_width, (GLsizei)afx->display_height);
#endif

    AvxCmdBindPipeline(dctx, 0, dev->pip, NIL, NIL);

    AvxCmdPushConstants(dctx, 0, sizeof(ortho), ortho);

    avxViewport vp = AVX_VIEWPORT(0, 0, afx->display_width, afx->display_height, 0, 1);
    AvxCmdAdjustViewports(dctx, 0, 1, &vp);

    {
        /* convert from command queue into draw list and draw to screen */
        const struct nk_draw_command *cmd;
        void *vertices, *elements;
        nk_size offset = 0;

        /* allocate vertex and element buffer */
#if 0
        glBindVertexArray(dev->vao);
        glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

        glBufferData(GL_ARRAY_BUFFER, max_vertex_buffer, NULL, GL_STREAM_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, max_element_buffer, NULL, GL_STREAM_DRAW);

        /* load draw vertices & elements directly into vertex + element buffer */
        vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
#endif
        vertices = dev->mappedVbo;
        elements = dev->mappedIbo;

        {
            /* fill convert configuration */
            struct nk_convert_config config;
            static const struct nk_draw_vertex_layout_element vertex_layout[] = {
                {NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_afx_vertex, position)},
                {NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_afx_vertex, uv)},
                {NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_afx_vertex, col)},
                {NK_VERTEX_LAYOUT_END}
            };
            memset(&config, 0, sizeof(config));
            config.vertex_layout = vertex_layout;
            config.vertex_size = sizeof(struct nk_afx_vertex);
            config.vertex_alignment = NK_ALIGNOF(struct nk_afx_vertex);
            config.tex_null = dev->tex_null;
            config.circle_segment_count = 22;
            config.curve_segment_count = 22;
            config.arc_segment_count = 22;
            config.global_alpha = 1.0f;
            config.shape_AA = AA;
            config.line_AA = AA;

            /* setup buffers to load vertices and elements */
            nk_buffer_init_fixed(&vbuf, vertices, (size_t)max_vertex_buffer);
            nk_buffer_init_fixed(&ebuf, elements, (size_t)max_element_buffer);
            nk_convert(&afx->ctx, &dev->cmds, &vbuf, &ebuf, &config);
        }

        avxBufferedMap maps[2] = { 0 };
        maps[0].buf = dev->vbo;
        maps[0].range = dev->vboSiz;
        maps[1].buf = dev->ebo;
        maps[1].range = dev->iboSiz;
        AvxFlushBufferedMaps(dev->dsys, 2, maps);

        avxBufferedStream stream = { 0 };
        stream.buf = dev->vbo;
        stream.range = dev->vboSiz;
        stream.stride = sizeof(struct nk_afx_vertex);
        AvxCmdBindVertexBuffers(dctx, 0, 1, &stream);
        AvxCmdBindIndexBuffer(dctx, dev->ebo, 0, dev->iboSiz, sizeof(afxUnit16));
#if 0
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif

        /* iterate over and execute each draw command */
        nk_draw_foreach(cmd, &afx->ctx, &dev->cmds)
        {
            if (!cmd->elem_count) continue;

#if 0
            glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
#endif
#if 0
            AvxCmdBindSamplers(dctx, 0, 0, 1, &cmd->texture.ptr); // <<<<<<<<<<<<<<
#else
            AvxCmdBindSamplers(dctx, 0, 0, 1, &dev->font_samp); // <<<<<<<<<<<<<<
#endif
            AvxCmdBindRasters(dctx, 0, 0, 1, &cmd->texture.ptr); // <<<<<<<<<<<<<<

#if 0
            glScissor(
                (GLint)(cmd->clip_rect.x * afx->fb_scale.x),
                (GLint)((afx->height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * afx->fb_scale.y),
                (GLint)(cmd->clip_rect.w * afx->fb_scale.x),
                (GLint)(cmd->clip_rect.h * afx->fb_scale.y));
#endif

#if 0
            afxRect rc = AVX_RECT((cmd->clip_rect.x * glfw->fb_scale.x),
                                ((glfw->height - (afxInt)(cmd->clip_rect.y + cmd->clip_rect.h)) * glfw->fb_scale.y),
                                (cmd->clip_rect.w * glfw->fb_scale.x),
                                (cmd->clip_rect.h * glfw->fb_scale.y));
#else
            afxRect rc = AVX_RECT(  (NK_MAX(cmd->clip_rect.x * scale[0], 0.f)),
                                    (NK_MAX((afx->height - (cmd->clip_rect.y + cmd->clip_rect.h)) * scale[1], 0.f)), // we must invert this to originate at bottom.
                                    (cmd->clip_rect.w * scale[0]),
                                    (cmd->clip_rect.h * scale[1]));
#endif
            AvxCmdAdjustScissors(dctx, 0, 1, &rc);

#if 0
            glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, (const void*)offset);
            offset += cmd->elem_count * sizeof(nk_draw_index);
#endif
#if 0
            AvxCmdDrawIndexed(dctx, cmd->elem_count, 1, 0, offset, 0);
            offset += cmd->elem_count * sizeof(nk_draw_index);
#else
            AvxCmdDrawIndexed(dctx, cmd->elem_count, 1, offset, 0, 0);
            offset += cmd->elem_count;
#endif
        }
        nk_clear(&afx->ctx);
        nk_buffer_clear(&dev->cmds);
    }
#if 0
    /* default OpenGL state */
    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
#endif
}

#if 0
NK_API void
nk_afx_char_callback(afxWindow win, unsigned int codepoint)
{
    struct nk_afx* afx = (struct nk_afx *)afxGetWindowUserPointer(win);
    if (afx->text_len < NK_AFX_TEXT_MAX)
        afx->text[afx->text_len++] = codepoint;
}

NK_API void
nk_afx_scroll_callback(afxWindow win, double xoff, double yoff)
{
    struct nk_afx* afx = (struct nk_afx *)afxGetWindowUserPointer(win);
    (void)xoff;
    afx->scroll.x += (float)xoff;
    afx->scroll.y += (float)yoff;
}

NK_API void
nk_afx_mouse_button_callback(afxWindow win, int button, int action, int mods)
{
    struct nk_afx* afx = (struct nk_afx *)AfxGetWindowUdd(win);
    double x, y;
    NK_UNUSED(mods);
    if (button != AFX_MOUSE_BUTTON_LEFT) return;
    afxGetCursorPos(win, &x, &y);
    if (action == AFX_PRESS)  {
        double dt = afxGetTime() - afx->last_button_click;
        if (dt > NK_AFX_DOUBLE_CLICK_LO && dt < NK_AFX_DOUBLE_CLICK_HI) {
            afx->is_double_click_down = nk_true;
            afx->double_click_pos = nk_vec2((float)x, (float)y);
        }
        afx->last_button_click = afxGetTime();
    } else afx->is_double_click_down = nk_false;
}
#endif

NK_INTERN void
nk_afx_clipboard_paste(nk_handle usr, struct nk_text_edit *edit)
{
    struct nk_afx* afx = (struct nk_afx*)usr.ptr;
    afxString4096 s;
    AfxMakeString4096(&s, NIL);
    afxUnit len = AfxGetClipboardContent(&s.s);
    if (len) nk_textedit_paste(edit, s.s.start, s.s.len);
    (void)usr;
}

NK_INTERN void
nk_afx_clipboard_copy(nk_handle usr, const char *text, int len)
{
    struct nk_afx* afx = (struct nk_afx*)usr.ptr;
    char *str = 0;
    if (!len) return;
    str = (char*)malloc((size_t)len+1);
    if (!str) return;
    memcpy(str, text, (size_t)len);
    str[len] = '\0';
    afxString s;
    AfxMakeString(&s, 0, str, 0);
    AfxSetClipboardContent(&s);
    free(str);
}

NK_API struct nk_context*
nk_afx_init(struct nk_afx* afx, afxWindow win, enum nk_afx_init_state init_state)
{
    afxDrawOutput dout;
    AfxGetWindowDrawOutput(win, FALSE, &dout);
    afx->ogl.dsys = AvxGetDrawOutputContext(dout);

    afx->ogl.vboSiz = MAX_VERTEX_BUFFER;
    afx->ogl.iboSiz = MAX_ELEMENT_BUFFER;

    afx->win = win;
#if 0
    if (init_state == NK_AFX_INSTALL_CALLBACKS) {
        afxSetScrollCallback(win, nk_afx_scroll_callback);
        afxSetCharCallback(win, nk_afx_char_callback);
        afxSetMouseButtonCallback(win, nk_afx_mouse_button_callback);
    }
#endif
    nk_init_default(&afx->ctx, 0);
    afx->ctx.clip.copy = nk_afx_clipboard_copy;
    afx->ctx.clip.paste = nk_afx_clipboard_paste;
    afx->ctx.clip.userdata = nk_handle_ptr(&afx);
    afx->last_button_click = 0;
    nk_afx_device_create(afx);

    afx->is_double_click_down = nk_false;
    afx->double_click_pos = nk_vec2(0, 0);

    afx->delta_time_seconds_last = (float)AfxGetTimer();

    return &afx->ctx;
}

NK_API void
nk_afx_font_stash_begin(struct nk_afx* afx, struct nk_font_atlas **atlas)
{
    nk_font_atlas_init_default(&afx->atlas);
    nk_font_atlas_begin(&afx->atlas);
    *atlas = &afx->atlas;
}

NK_API void
nk_afx_font_stash_end(struct nk_afx* afx)
{
    const void *image; int w, h;
    image = nk_font_atlas_bake(&afx->atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
    nk_afx_device_upload_atlas(afx, image, w, h);
    nk_font_atlas_end(&afx->atlas, nk_handle_id((int)afx->ogl.font_tex), &afx->ogl.tex_null);
    if (afx->atlas.default_font)
        nk_style_set_font(&afx->ctx, &afx->atlas.default_font->handle);
}

NK_API void
nk_afx_new_frame(struct nk_afx* afx)
{
    int i;
    double x, y;
    struct nk_context *ctx = &afx->ctx;
    afxWindow win = afx->win;

    /* update the timer */
    float delta_time_now = (float)AfxGetTimer();
    afx->ctx.delta_time_seconds = delta_time_now - afx->delta_time_seconds_last;
    afx->delta_time_seconds_last = delta_time_now;

    afxRect rc;
    AfxGetWindowRect(win, NIL, &rc); // Nuklear can't handle input out of surface when it can't draw the entire window, including its frame.
    afx->width = rc.w;
    afx->height = rc.h;
    AfxGetWindowRect(win, FALSE, &rc);
    afx->display_width = rc.w;
    afx->display_height = rc.h;
    afx->fb_scale.x = (float)afx->display_width/(float)afx->width;
    afx->fb_scale.y = (float)afx->display_height/(float)afx->height;

    nk_input_begin(ctx);
    for (i = 0; i < afx->text_len; ++i)
        nk_input_unicode(ctx, afx->text[i]);

#ifdef NK_AFX_MOUSE_GRABBING
    /* optional grabbing behavior */
    if (ctx->input.mouse.grab)
        afxSetInputMode(afx.win, AFX_CURSOR, AFX_CURSOR_HIDDEN);
    else if (ctx->input.mouse.ungrab)
        afxSetInputMode(afx->win, AFX_CURSOR, AFX_CURSOR_NORMAL);
#endif

    afxSession ses;
    AfxGetSession(&ses);

    nk_input_key(ctx, NK_KEY_DEL, AfxIsKeyPressed(0, afxKey_DELETE));
    nk_input_key(ctx, NK_KEY_ENTER, AfxIsKeyPressed(0, afxKey_RET));
    nk_input_key(ctx, NK_KEY_TAB, AfxIsKeyPressed(0, afxKey_TAB));
    nk_input_key(ctx, NK_KEY_BACKSPACE, AfxIsKeyPressed(0, afxKey_BACK));
    nk_input_key(ctx, NK_KEY_UP, AfxIsKeyPressed(0, afxKey_UP));
    nk_input_key(ctx, NK_KEY_DOWN, AfxIsKeyPressed(0, afxKey_DOWN));
    nk_input_key(ctx, NK_KEY_TEXT_START, AfxIsKeyPressed(0, afxKey_HOME));
    nk_input_key(ctx, NK_KEY_TEXT_END, AfxIsKeyPressed(0, afxKey_END));
    nk_input_key(ctx, NK_KEY_SCROLL_START, AfxIsKeyPressed(0, afxKey_HOME));
    nk_input_key(ctx, NK_KEY_SCROLL_END, AfxIsKeyPressed(0, afxKey_END));
    nk_input_key(ctx, NK_KEY_SCROLL_DOWN, AfxIsKeyPressed(0, afxKey_PAGEDN));
    nk_input_key(ctx, NK_KEY_SCROLL_UP, AfxIsKeyPressed(0, afxKey_PAGEUP));
    nk_input_key(ctx, NK_KEY_SHIFT, AfxIsKeyPressed(0, afxKey_LSHIFT));

    if (AfxIsKeyPressed(0, afxKey_LCTRL) ||
        AfxIsKeyPressed(0, afxKey_LCTRL)) {
        nk_input_key(ctx, NK_KEY_COPY, AfxIsKeyPressed(0, afxKey_C));
        nk_input_key(ctx, NK_KEY_PASTE, AfxIsKeyPressed(0, afxKey_V));
        nk_input_key(ctx, NK_KEY_CUT, AfxIsKeyPressed(0, afxKey_X));
        nk_input_key(ctx, NK_KEY_TEXT_UNDO, AfxIsKeyPressed(0, afxKey_Z));
        nk_input_key(ctx, NK_KEY_TEXT_REDO, AfxIsKeyPressed(0, afxKey_R));
        nk_input_key(ctx, NK_KEY_TEXT_WORD_LEFT, AfxIsKeyPressed(0, afxKey_LEFT));
        nk_input_key(ctx, NK_KEY_TEXT_WORD_RIGHT, AfxIsKeyPressed(0, afxKey_RIGHT));
        nk_input_key(ctx, NK_KEY_TEXT_LINE_START, AfxIsKeyPressed(0, afxKey_B));
        nk_input_key(ctx, NK_KEY_TEXT_LINE_END, AfxIsKeyPressed(0, afxKey_E));
    } else {
        nk_input_key(ctx, NK_KEY_LEFT, AfxIsKeyPressed(0, afxKey_LEFT));
        nk_input_key(ctx, NK_KEY_RIGHT, AfxIsKeyPressed(0, afxKey_RIGHT));
        nk_input_key(ctx, NK_KEY_COPY, 0);
        nk_input_key(ctx, NK_KEY_PASTE, 0);
        nk_input_key(ctx, NK_KEY_CUT, 0);
    }

    afxRect cursRect;
    AfxGetCursorPlacement(NIL, win, NIL, &cursRect);
    nk_input_motion(ctx, (int)cursRect.x, (int)cursRect.y);
    x = cursRect.x;
    y = cursRect.y;
#ifdef NK_AFX_MOUSE_GRABBING
    if (ctx->input.mouse.grabbed) {
        afxSetCursorPos(afx->win, ctx->input.mouse.prev.x, ctx->input.mouse.prev.y);
        ctx->input.mouse.pos.x = ctx->input.mouse.prev.x;
        ctx->input.mouse.pos.y = ctx->input.mouse.prev.y;
    }
#endif
    nk_input_button(ctx, NK_BUTTON_LEFT, (int)x, (int)y, AfxIsLmbPressed(0));
    nk_input_button(ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, AfxIsMmbPressed(0));
    nk_input_button(ctx, NK_BUTTON_RIGHT, (int)x, (int)y, AfxIsRmbPressed(0));
    nk_input_button(ctx, NK_BUTTON_DOUBLE, (int)afx->double_click_pos.x, (int)afx->double_click_pos.y, afx->is_double_click_down);
    nk_input_scroll(ctx, afx->scroll);
    nk_input_end(&afx->ctx);
    afx->text_len = 0;
    afx->scroll = nk_vec2(0,0);
}

NK_API
void nk_afx_shutdown(struct nk_afx* afx)
{
    nk_font_atlas_clear(&afx->atlas);
    nk_free(&afx->ctx);
    nk_afx_device_destroy(afx);
    memset(afx, 0, sizeof(*afx));
}

#endif
