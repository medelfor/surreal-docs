/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxGenerateView.h"
#include <fmt/format.h>
#include <sstream>
#include <ftxui/component/component.hpp>
#include "udocs-processor/cli/CLISignalHandler.h"

#define NOLINT()

void udocs_processor::FtxGenerateView::SetStatus(
    GenerateView::Status Status_, const std::string &Message) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  if (Status_ == GenerateView::Status::ERROR) {
    StatusString = GetStatusString(Status_);
    this->Status_ = Status_;
    HasErrored = true;
    ErrorMessage = Message;
  } else if (this->Status_ != GenerateView::Status::ERROR) {
    StatusString = GetStatusString(Status_);
    this->Status_ = Status_;
    Progress = static_cast<uint32_t>(Status_) / static_cast<float>(
        static_cast<uint32_t>(Status::ERROR) - 1);
  }
}

void udocs_processor::FtxGenerateView::Destroy() {
  Screen.Exit();
}

void udocs_processor::FtxGenerateView::Tick() {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  FtxView::Tick();

  ++FrameCount;
  if (AdFrames == AD_FRAMES) {
    Ad = (Ad + 1) % AdTexts.size();
    AdFrames = 0;
  }
  ++AdFrames;
  Screen.PostEvent(ftxui::Event::Custom);
}

ftxui::Elements udocs_processor::FtxGenerateView::Split(
    const std::string& Text) const {
  ftxui::Elements Output;
  std::stringstream Stream(Text);
  std::string Word;
  while (std::getline(Stream, Word))
    Output.push_back(ftxui::text(Word));
  return Output;
}

void udocs_processor::FtxGenerateView::Init() {
  FtxView::Init();

  using namespace ftxui; NOLINT()

  List = {"", "", ""};

  ExitOnFinish = Container::Vertical({
      Checkbox("Exit Surreal Docs when finished", &DoExit_) | ForegroundColor1()
  });

  AdsRadio = Container::Horizontal({
      Radiobox(&List, &Ad) | ForegroundColor1()
  });

  Renderer = ftxui::Renderer(Container::Vertical({ExitOnFinish,
      AdsRadio, GetComponents()}),
   [&]() {
     return WrapInWindow({
         window(
             hbox({
                 Gap(), text("Status") | ForegroundColor1()
             }),
             hbox({
                 text("[ FAIL ]") | ErroneuousColor()
                     | HHide(!HasFinished || !HasErrored),
                 text("[  OK  ]") | SuccessColor()
                     | HHide(!HasFinished || HasErrored),
                 spinner(18, FrameCount) | SuccessColor()
                     | HHide(HasErrored || HasFinished),
                 separatorEmpty(),
                 text(StatusString + (HasFinished ? PRESS_TO_EXIT : ""))
                     | ForegroundColor1(),
                 filler(),
                 text(fmt::format("{:.0f}%", round(100 * Progress))) |
                     ForegroundColor1(),
                 separatorEmpty(),
                 gauge(Progress) | size(WIDTH, Constraint::EQUAL, 35)
                     | ForegroundColor1(),
                 separatorEmpty()
             })),
         hbox({
             Gap(1),
             ExitOnFinish->Render()
         }),
         filler(),
         hbox(
             filler(),
             hbox(
                 vbox({
                     filler(),
                     vbox({
                         Split(AD_IMAGE)
                     }) | color(AdColor(Ad)),
                     filler()
                 }),
                 separatorEmpty(),
                 separatorEmpty(),
                 separatorEmpty(),
                 vbox({
                     separatorEmpty(),
                     hflow(
                         paragraph(AdText(Ad))
                            | size(ftxui::WIDTH, ftxui::LESS_THAN, 50))
                 }) | ftxui::vcenter | ForegroundColor1()),
             filler(),
             vbox({
                 separatorEmpty(),
                 AdsRadio->Render()
             }),
             separatorEmpty()) | VHide(HasErrored),
         filler(),
         WrapError(ErrorMessage, HasErrored)
         }, "Generation");
     });
  Renderer |= CatchEvent([&](Event Event_) {
    if (HasFinished && Event_ == Event::Escape) {
      DoExit_ = true;
      this->Screen.Exit();
      return true;
    }
    return false;
  });

  CLISignalHandler::OverrideHandler();
}

std::string udocs_processor::FtxGenerateView::AdText(int Ad) const {
  return AdTexts[Ad];
}

ftxui::Color udocs_processor::FtxGenerateView::AdColor(int Ad) const {
  return AdColors[Ad];
}

udocs_processor::FtxGenerateView::FtxGenerateView()
    : Screen(ftxui::ScreenInteractive::Fullscreen()) {
  AdTexts.emplace_back(AD_0);
  AdTexts.emplace_back(AD_1);
  AdTexts.emplace_back(AD_2);

  AdColors.emplace_back(AD_0_COLOR);
  AdColors.emplace_back(AD_1_COLOR);
  AdColors.emplace_back(AD_2_COLOR);
}

bool udocs_processor::FtxGenerateView::DoExit() const {
  return DoExit_ && HasFinished;
}

void udocs_processor::FtxGenerateView::SetFinished(bool HasFinished) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->HasFinished = HasFinished;
}

void udocs_processor::FtxGenerateView::Start() {
  Screen.Loop(Renderer);
}
