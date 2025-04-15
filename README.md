# 🧩 Nuklear Immediate UI on Qwadro

This project integrates the [Nuklear Immediate Mode UI](https://github.com/Immediate-Mode-UI/Nuklear) with [Qwadro](https://sigmaco.org/qwadro) — a execution ecosystem API that provides **shell-integrated windows**, **user input**, and **hardware-accelerated graphics**.

It also features a flexible **custom background rendering system** that allows you to draw visuals beneath the UI, opening up creative possibilities for tools, overlays, and dynamic interfaces.

Actually, there is not synchronization occuring. That is why there are VRAM corruption artifacts and throttling present.
The GPU synchronization mechanisms are ready to be used but they are not because a unreliable implementation behavior still has not been fixed.

---

## ⚙️ What is Qwadro?

**[Qwadro](https://sigmaco.org/qwadro)** is a modular C/C++ API that provides the system-level building blocks for responsive native applications:

- 🪟 Native shell-integrated window creation  
- 🎮 Controller and HID input handling  
- ⚡ Fast GPU-accelerated graphics operations

Qwadro acts as the platform abstraction layer that powers this implementation.

---

## ✨ Features

- 🔲 **Immediate Mode UI** using Nuklear  
- 🖼️ **Custom background rendering** via Installable Client Driver.
- 💻 **Shell integration** via Qwadro Multimedia UX Infrastructure
- 🚀 **Hardware-accelerated rendering** using Qwadro Video Graphics Infrastructure

---

## 🛠️ Example Usage

```c
// Draw custom background with Qwadro
q_draw_custom_background();

// Begin Nuklear UI frame
if (nk_begin(ctx, "Control Panel", nk_rect(60, 60, 320, 240), NK_WINDOW_TITLE)) {
    nk_layout_row_dynamic(ctx, 30, 1);
    nk_label(ctx, "Welcome to Qwadro UI!", NK_TEXT_LEFT);
}
nk_end(ctx);


Get help at gathering of developers on Discord.
[![SIGMA Discord Server](https://discord.com/api/guilds/349379672351571969/widget.png?style=banner2)](https://sigmaco.org/discord)
