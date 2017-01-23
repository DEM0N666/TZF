// ImGui - standalone example application for DirectX 9
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "imgui/imgui.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#include "config.h"
#include "render.h"
#include "framerate.h"
#include "hook.h"

#include <string>
#include <vector>


bool show_config      = true;
bool show_test_window = false;

ImVec4 clear_col = ImColor(114, 144, 154);

struct {
  std::vector <const char*> array;
  int                       sel   = 0;
} gamepads;

void
__TZF_DefaultSetOverlayState (bool)
{
}

void
TZFix_PauseGame (bool pause)
{
  typedef void (__stdcall *SK_SteamAPI_SetOverlayState_pfn)(bool active);

  static SK_SteamAPI_SetOverlayState_pfn
    SK_SteamAPI_SetOverlayState =
      (SK_SteamAPI_SetOverlayState_pfn)

    TZF_ImportFunctionFromSpecialK (
      "SK_SteamAPI_SetOverlayState",
        &__TZF_DefaultSetOverlayState
    );

  SK_SteamAPI_SetOverlayState (pause);
}

int cursor_refs = 0;

void
TZFix_ToggleConfigUI (void)
{
  static int cursor_refs = 0;

  if (config.control_panel.visible) {
    while (ShowCursor (FALSE) > cursor_refs)
      ;
    while (ShowCursor (TRUE) < cursor_refs)
      ;
  }
  else {
    cursor_refs = (ShowCursor (FALSE) + 1);

    if (cursor_refs > 0) {
      while (ShowCursor (FALSE) > 0)
        ;
    }
  }

  config.control_panel.visible = (! config.control_panel.visible);

  SK_GetCommandProcessor ()->ProcessCommandLine (
    config.control_panel.visible ? "ImGui.Visible 1" :
                                   "ImGui.Visible 0"
    );

  if (config.control_panel.auto_pause)
    TZFix_PauseGame (config.control_panel.auto_pause);
}


void
TZFix_GamepadConfigDlg (void)
{
#if 0
  if (gamepads.array.size () == 0)
  {
    if (GetFileAttributesA ("TBFix_Res\\Gamepads\\") != INVALID_FILE_ATTRIBUTES)
    {
      std::vector <std::string> gamepads_;

      WIN32_FIND_DATAA fd;
      HANDLE           hFind  = INVALID_HANDLE_VALUE;
      int              files  = 0;
      LARGE_INTEGER    liSize = { 0 };

      hFind = FindFirstFileA ("TBFix_Res\\Gamepads\\*", &fd);

      if (hFind != INVALID_HANDLE_VALUE)
      {
        do {
          if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
               fd.cFileName[0]    != '.' )
          {
            gamepads_.push_back (fd.cFileName);
          }
        } while (FindNextFileA (hFind, &fd) != 0);

        FindClose (hFind);
      }

            char current_gamepad [128] = { '\0' };
      snprintf ( current_gamepad, 127,
                   "%ws",
                     config.input.gamepad.texture_set.c_str () );

      for (int i = 0; i < gamepads_.size (); i++)
      {
        gamepads.array.push_back (
          _strdup ( gamepads_ [i].c_str () )
        );

        if (! stricmp (gamepads.array [i], current_gamepad))
          gamepads.sel = i;
      }
    }
  }

  if (ImGui::BeginPopupModal ("Gamepad Config", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_ShowBorders))
  {
    int orig_sel = gamepads.sel;

    if (ImGui::ListBox ("Gamepad\nIcons", &gamepads.sel, gamepads.array.data (), gamepads.array.size (), 3))
    {
      if (orig_sel != gamepads.sel)
      {
        wchar_t pad [128] = { L'\0' };

        swprintf (pad, L"%hs", gamepads.array [gamepads.sel]);

        config.input.gamepad.texture_set    = pad;
        tzf::RenderFix::need_reset.textures = true;
      }

      ImGui::CloseCurrentPopup ();
    }

    ImGui::EndPopup ();
  }
#endif
}

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

IMGUI_API
void
ImGui_ImplDX9_NewFrame (void);


void
TZFix_DrawConfigUI (void)
{
  ImGui_ImplDX9_NewFrame ();

  ImGuiIO& io =
    ImGui::GetIO();

  ImGui::SetNextWindowPosCenter       (ImGuiSetCond_Always);
  ImGui::SetNextWindowSizeConstraints (ImVec2 (50, 50), ImGui::GetIO ().DisplaySize);

  // 1. Show a simple window
  // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    ImGui::Begin ( "Tales of Zestiria \"Fix\" Control Panel",
                     &show_config,
                      ( ImGuiWindowFlags_AlwaysAutoResize |
                        ImGuiWindowFlags_ShowBorders      |
                        ImGuiWindowFlags_AlwaysUseWindowPadding )
                 );

    ImGui::PushItemWidth (ImGui::GetWindowWidth () * 0.65f);

    if (tzf::RenderFix::need_reset.graphics || tzf::RenderFix::need_reset.textures) {
      ImGui::TextColored ( ImVec4 (0.8f, 0.8f, 0.1f, 1.0f),
                             "    You have made changes that will not apply until you change Screen Modes in Graphics Settings,\n"
                             "      or by performing Alt + Tab with the game set to Fullscreen mode." );
    }

    if (ImGui::CollapsingHeader ("Framerate Control", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen))
    {
#if 0
      int limiter =
        0;//config.framerate.replace_limiter ? 1 : 0;

      const char* szLabel = (limiter == 0 ? "Framerate Limiter  (choose something else!)" : "Framerate Limiter");

      ImGui::Combo (szLabel, &limiter, "Namco          (A.K.A. Stutterfest 2017)\0"
                                       "Special K      (Precision Timing For The Win!)\0\0" );
#endif

      static float values [120]  = { 0 };
      static int   values_offset =   0;

      values [values_offset] = 1000.0f * ImGui::GetIO ().DeltaTime;
      values_offset = (values_offset + 1) % IM_ARRAYSIZE (values);

//
// XXX
//
#if 0
      if (limiter != config.framerate.replace_limiter)
      {
        config.framerate.replace_limiter = limiter;

        if (config.framerate.replace_limiter)
          tbf::FrameRateFix::BlipFramerate    ();
        else
          tbf::FrameRateFix::DisengageLimiter ();

        float fZero = 0.0f;
        memset (values, *(reinterpret_cast <DWORD *> (&fZero)), sizeof (float) * 120);
        values_offset = 0;
      }
#endif

      float sum = 0.0f;

      float min = FLT_MAX;
      float max = 0.0f;

      for (float val : values) {
        sum += val;

        if (val > max)
          max = val;

        if (val < min)
          min = val;
      }

      static char szAvg [512];


      sprintf ( szAvg,
                  "Avg milliseconds per-frame: %6.3f  (Target: %6.3f)\n"
                  "    Extreme frametimes:      %6.3f min, %6.3f max\n\n\n\n"
                  "Variation:  %8.5f ms  ==>  %.1f FPS  +/-  %3.1f frames",
                    sum / 120.0f, tzf::FrameRateFix::GetTargetFrametime (),
                      min, max, max - min,
                        1000.0f / (sum / 120.0f), (max - min) / (1000.0f / (sum / 120.0f)) );

      ImGui::PlotLines     ( "",
                              values,
                                IM_ARRAYSIZE (values),
                                  values_offset,
                                    szAvg,
                                      0.0f,
                                        2.0f * tzf::FrameRateFix::GetTargetFrametime (),
                                          ImVec2 ( 600, 82 ) );
                          
      ImGui::SameLine     ();

      ImGui::PushItemWidth (210);
      ImGui::BeginChild   ( "Sub2", ImVec2 ( 210, 82 ), true );

      int cutscene = 0, battle = 0, world = 0;

      if (config.framerate.cutscene_target == 30) cutscene = 1; else cutscene = 0;

      if        (config.framerate.battle_target == 10)    battle = 5;
      else if   (config.framerate.battle_target == 12)    battle = 4;
      else if   (config.framerate.battle_target == 15)    battle = 3;
      else if   (config.framerate.battle_target == 20)    battle = 2;
      else if   (config.framerate.battle_target == 30)    battle = 1;
      else /*if (config.framerate.battle_target == 60)*/  battle = 0;

      if        (config.framerate.target == 10)           world = 5;
      else if   (config.framerate.target == 12)           world = 4;
      else if   (config.framerate.target == 15)           world = 3;
      else if   (config.framerate.target == 20)           world = 2;
      else if   (config.framerate.target == 30)           world = 1;
      else /*if (config.framerate.battle_target == 60)*/  world = 0;

      ImGui::Combo        ("Battle  ", &battle,   " 60 FPS\0 30 FPS\0 20 FPS\0 15 FPS\0 12 FPS\0 10 FPS\0\0");
      ImGui::Combo        ("Cutscene", &cutscene, " 60 FPS\0 30 FPS\0\0");
      ImGui::Combo        ("World   ", &world,    " 60 FPS\0 30 FPS\0 20 FPS\0 15 FPS\0 12 FPS\0 10 FPS\0\0");

      config.framerate.cutscene_target = (cutscene == 0) ? 60 : 30;

           if (world == 0) config.framerate.target = 60;
      else if (world == 1) config.framerate.target = 30;
      else if (world == 2) config.framerate.target = 20;
      else if (world == 3) config.framerate.target = 15;
      else if (world == 4) config.framerate.target = 12;
      else if (world == 5) config.framerate.target = 10;

           if (battle == 0) config.framerate.battle_target = 60;
      else if (battle == 1) config.framerate.battle_target = 30;
      else if (battle == 2) config.framerate.battle_target = 20;
      else if (battle == 3) config.framerate.battle_target = 15;
      else if (battle == 4) config.framerate.battle_target = 12;
      else if (battle == 5) config.framerate.battle_target = 10;

      ImGui::EndChild     ();
      ImGui::PopItemWidth ();

      //ImGui::Text ( "Application average %.3f ms/frame (%.1f FPS)",
                      //1000.0f / ImGui::GetIO ().Framerate,
                                //ImGui::GetIO ().Framerate );
    }

    if (ImGui::CollapsingHeader ("Texture Options"))
    {
      if (ImGui::Checkbox ("Dump Textures",    &config.textures.dump))     tzf::RenderFix::need_reset.graphics = true;
      if (ImGui::Checkbox ("Generate Mipmaps", &config.textures.remaster)) tzf::RenderFix::need_reset.graphics = true;

      if (ImGui::IsItemHovered ())
        ImGui::SetTooltip ("Eliminates distant texture aliasing");

#if 0
      if (config.textures.remaster) {
        ImGui::SameLine (150); 

        if (ImGui::Checkbox ("(Uncompressed)", &config.textures.uncompressed)) tbf::RenderFix::need_reset.graphics = true;

        if (ImGui::IsItemHovered ())
          ImGui::SetTooltip ("Uses more VRAM, but avoids texture compression artifacts on generated mipmaps.");
      }
#endif

      ImGui::SliderFloat ("LOD Bias", &config.textures.lod_bias, -3.0f, 3.0f);

      if (ImGui::IsItemHovered ())
      {
        ImGui::BeginTooltip ();
        ImGui::Text         ("Controls texture sharpness;  -3 = Sharpest (WILL shimmer),  0 = Neutral,  3 = Blurry");
        ImGui::EndTooltip   ();
      }
      

      ImGui::Checkbox("Display Texture Resampling Indicator in OSD", &config.textures.show_loading_text);
    }

#if 0
    if (ImGui::CollapsingHeader ("Shader Options"))
    {
      ImGui::Checkbox ("Dump Shaders", &config.render.dump_shaders);
    }
#endif

    if (ImGui::CollapsingHeader ("Aspect Ratio"))
    {
      ImGui::Checkbox ("Use Aspect Ratio Correction Everywhere",     &config.render.aspect_correction);
      ImGui::TreePush ("");
      ImGui::Checkbox ("Clear non-16:9 Region in Menus",             &config.render.clear_blackbars);
      ImGui::TreePop  ();

      if (! config.render.aspect_correction)
      {
          ImGui::Checkbox ("Apply Correction (only) to Bink Videos", &config.render.blackbar_videos);
      }
    }

    if (ImGui::CollapsingHeader ("Post-Processing"))
    {
      if ( ImGui::SliderFloat ("Post-Process Resolution Scale", &config.render.postproc_ratio, 0.0f, 2.0f) )
        tzf::RenderFix::need_reset.graphics = true;
    }

    if (ImGui::CollapsingHeader ("Shadow Quality"))
    {
      struct shadow_imp_s
      {
        shadow_imp_s (int scale)
        {
          scale = std::abs (scale);

          if (scale > 3)
            scale = 3;

          radio    = scale;
          last_sel = radio;
        }

        int radio    = 0;
        int last_sel = 0;
      };

      static shadow_imp_s shadows     (config.render.shadow_rescale);
      static shadow_imp_s env_shadows (config.render.env_shadow_rescale);

      ImGui::Combo ("Character Shadow Resolution",     &shadows.radio,     "Normal\0Enhanced\0High\0Ultra\0\0");
      ImGui::Combo ("Environmental Shadow Resolution", &env_shadows.radio, "Normal\0High\0Ultra\0\0");

      ImGui::TextColored ( ImVec4 ( 0.999f, 0.01f, 0.999f, 1.0f),
                             " * Changes to these settings will produce weird results until you change Screen Mode in-game...\n" );

      if (env_shadows.radio != env_shadows.last_sel) {
        config.render.env_shadow_rescale    = env_shadows.radio;
        env_shadows.last_sel                = env_shadows.radio;
        tzf::RenderFix::need_reset.graphics = true;
      }

      if (shadows.radio != shadows.last_sel) {
        config.render.shadow_rescale        = -shadows.radio;
        shadows.last_sel                    =  shadows.radio;
        tzf::RenderFix::need_reset.graphics = true;
      }
    }

    if (ImGui::CollapsingHeader ("Audio Configuration"))
    {
      ImGui::Checkbox ("Enable Audio Sample Rate Fix", &config.audio.enable_fix);

      if (config.audio.enable_fix)
      {
        int sel;

        if   (config.audio.sample_hz == 48000)  sel = 1;
        else                                    sel = 0;

        ImGui::Combo ("Sample Rate", &sel, " 44.1 kHz\0 48.0 kHz\0\0", 2);

        if (sel == 0)  config.audio.sample_hz = 44100;
        else           config.audio.sample_hz = 48000;

        ImGui::Checkbox ("Use Compatibility Mode", &config.audio.compatibility);

        if (ImGui::IsItemHovered ())
          ImGui::SetTooltip ("May reduce audio quality, but can help with some weird USB headsets and Windows 7 / Older.");
      }
    }

    //ImGui::Separator ();

#if 0
    if (ImGui::Button ("Gamepad Config"))
      ImGui::OpenPopup ("Gamepad Config");

    TZFix_GamepadConfigDlg ();

    ImGui::SameLine ();
#endif

    //if (ImGui::Button ("Special K Config"))
      //show_gamepad_config ^= 1;

    //if ( ImGui::Checkbox ("Pause Game While This Menu Is Open", &config.control_panel.auto_pause) )
      //TZFix_PauseGame (config.control_panel.auto_pause);

    //ImGui::SameLine (500);

    if (ImGui::Selectable ("...", show_test_window))
      show_test_window = (! show_test_window);

    ImGui::End ();

    if (show_test_window) {
      ImGui::SetNextWindowPos (ImVec2 (650, 20), ImGuiSetCond_FirstUseEver);
      ImGui::ShowTestWindow   (&show_test_window);
    }

  // Rendering
  tzf::RenderFix::pDevice->SetRenderState (D3DRS_ZENABLE,           false);
  tzf::RenderFix::pDevice->SetRenderState (D3DRS_ALPHABLENDENABLE,  false);
  tzf::RenderFix::pDevice->SetRenderState (D3DRS_SCISSORTESTENABLE, false);

  if ( SUCCEEDED (
         tzf::RenderFix::pDevice->BeginScene ()
       )
     )
  {
    ImGui::Render                     ();
    tzf::RenderFix::pDevice->EndScene ();
  }
}