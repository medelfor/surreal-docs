/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include <ftxui/component/screen_interactive.hpp>
#include "SimpleView.h"
#include "FtxView.h"

namespace udocs_processor {
class FtxSimpleView : public virtual SimpleView, public virtual FtxView {
 public:
  FtxSimpleView();

  void Init() override;

  void Tick() override;

  void Start() override;

  void Destroy() override;

  bool DoExit() const override;

  void SetFinished(bool IsFinished) override;

  void ReportError(const std::string &Message) override;

  void ReportWarning(const std::string &Message) override;

  void ShowDescription(const std::string& Description) override;

  ~FtxSimpleView() override = default;

 protected:
  enum class State {
    SUCCESS,
    WARNING,
    FAILURE,
    PROGRESS
  };

  bool Confirm(const std::string &Message) override;

  void ReportProgress(const std::string &Message) override;

  void ReportSuccess(const std::string &Message) override;

  void ShowList(std::vector<std::string> List) override;

  std::mutex SelectionsProtection;

  ftxui::Component Renderer;
  ftxui::Component ConfirmationInputComponent;
  ftxui::Element ListElement;
  std::vector<ftxui::Element> ListElements;
  ftxui::ScreenInteractive Screen;
  uint32_t FrameCount = 0;

  State CurrentState = State::PROGRESS;
  std::string StateMessage;
  std::string Description;

  std::vector<std::string> List;

  bool HasList = false;
  bool IsFinished = false;

  bool DoConfirm = false;
  std::string ConfirmationInput;
  std::string ConfirmationMessage;
  std::mutex ConfirmationMutex;
  std::condition_variable ConfirmationCv;

  static constexpr const char CONFIRM_YES[] = "y";
  static constexpr const char CONFIRM_NO[] = "n";
  static constexpr const char CONFIRM_FORMAT[] = "{} [y/n]:";

  static constexpr const char LIST_GLUE[] = "\n\n";
};
}  // namespace udocs_processor
