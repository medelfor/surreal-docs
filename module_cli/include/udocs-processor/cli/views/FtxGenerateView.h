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

  ~FtxGenerateView() override = default;

 private:
  ftxui::Elements Split(const std::string& Text) const;

  std::string AdIcon(int Ad) const;

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

  static constexpr int AD_FRAMES = 60;

  static constexpr const char PRESS_TO_EXIT[] = ". Press ESC to exit";
};
}  // namespace udocs_processor
