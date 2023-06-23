/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <mutex> NOLINT()
#include <memory>
#include <string>
#include <vector>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "FtxView.h"
#include "GenerateView.h"

namespace udocs_processor {
class FtxGenerateView : public GenerateView, public FtxView {
 public:
  FtxGenerateView();

  void Init() override;

  void SetStatus(Status Status_, const std::string &Message) override;

  void Tick() override;

  bool DoExit() const override;

  void SetFinished(bool HasFinished) override;

  void Destroy() override;

  void Start() override;

 private:
  ftxui::Elements Split(const std::string& Text) const;

  std::string AdText(int Ad) const;

  ftxui::Color AdColor(int Ad) const;

  ftxui::Component ExitOnFinish;
  ftxui::Component AdsRadio;
  std::mutex SelectionsProtection;
  std::string StatusString;
  ftxui::Component Renderer;
  ftxui::ScreenInteractive Screen;
  uint32_t FrameCount = 0;
  int Ad = 0;
  int AdFrames = 0;
  std::vector<std::string> List;
  bool DoExit_ = false;
  bool HasFinished = false;
  float Progress = 0;
  bool HasErrored = false;
  std::string ErrorMessage;
  Status Status_ = Status::PREPARING;

  std::vector<std::string> AdTexts;
  std::vector<ftxui::Color> AdColors;

  static constexpr int AD_FRAMES = 50;
  const ftxui::Color AD_0_COLOR = ftxui::Color::DeepPink2;
  const ftxui::Color AD_1_COLOR = ftxui::Color::Turquoise2;
  const ftxui::Color AD_2_COLOR = ftxui::Color::Purple3;

  static constexpr const char PRESS_TO_EXIT[] = ". Press ESC to exit";

  static constexpr const char AD_0[] =
      "Need a cloud to make your docs available "
      "on the internet? Try Surreal Cloud starting "
      "at $17/month. More info at medelfor.com";
  static constexpr const char AD_1[] =
      "Need a cloud to share your docs privately with the team? "
      "Try Surreal Cloud+ starting "
      "at $25/month. More info at medelfor.com";
  static constexpr const char AD_2[] =
      "Need an on-premises solution to share your docs within organization? "
      "Try Surreal Cloud Pro starting "
      "at $70/month. More info at medelfor.com";

  static constexpr const char AD_IMAGE[] =
R"(  ▄▄▄▄▄▄
▄████████▄
██████████
 ▀██████▀
█▄      ▄█
 ▀██████▀
█▄      ▄█
 ▀██████▀ )";
};
}  // namespace udocs_processor
