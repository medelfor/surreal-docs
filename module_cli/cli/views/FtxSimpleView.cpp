/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/cli/views/FtxSimpleView.h"
#include "udocs-processor/cli/CLISignalHandler.h"

#define NOLINT()
bool al;
void udocs_processor::FtxSimpleView::Init() {
  FtxView::Init();

  using namespace ftxui; NOLINT()

  ListElement = ftxui::vbox(ListElements);

  ConfirmationInputComponent = ftxui::Input(&ConfirmationInput, "");

  Renderer = ftxui::Renderer(
      Container::Vertical({ConfirmationInputComponent}),
      [&]() {
        return vbox({
          hbox({
            text("[ WARN ]") | WarningColor()
                | HHide(CurrentState != State::WARNING),
            text("[ FAIL ]") | ErroneuousColor()
                | HHide(CurrentState != State::FAILURE),
            text("[  OK  ]") | SuccessColor()
                | HHide(CurrentState != State::SUCCESS),
            spinner(2, FrameCount) | SuccessColor()
                | HHide(CurrentState != State::PROGRESS),
            separatorEmpty(),
            text(StateMessage) | ForegroundColor1()
          }),
          vbox({
            separatorEmpty(),
            hflow(paragraph(Description)) | ForegroundColor1(),
            separatorEmpty(),
          }) | VHide(Description.empty()),
          vbox({
            separatorEmpty(),
            ftxui::Renderer([this]() {
              return ftxui::vbox(ListElements);
            })->Render(),
            separatorEmpty()
          }) | VHide(!HasList) | ForegroundColor1(),
          vbox({
            separatorEmpty(),
            hbox({
              hflow({
                paragraph(ConfirmationMessage),
                separatorEmpty(),
                ConfirmationInputComponent->Render()
                    | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 1)
              })
            }),
            separatorEmpty()
          }) | VHide(!DoConfirm) | ForegroundColor1(),
        });
      });

  CLISignalHandler::OverrideHandler();
}

void udocs_processor::FtxSimpleView::Tick() {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  FtxView::Tick();

  if (ConfirmationInput == CONFIRM_YES || ConfirmationInput == CONFIRM_NO) {
    std::unique_lock<std::mutex> Lock{ConfirmationMutex};
    ConfirmationCv.notify_all();
  } else {
    ConfirmationInput.clear();
  }

  ++FrameCount;
  Screen.PostEvent(ftxui::Event::Custom);
}

void udocs_processor::FtxSimpleView::Start() {
  Screen.Loop(Renderer);
  Screen.WithRestoredIO([](){});
}

void udocs_processor::FtxSimpleView::Destroy() {
  Screen.Exit();
}

bool udocs_processor::FtxSimpleView::DoExit() const {
  return IsFinished;
}

void udocs_processor::FtxSimpleView::SetFinished(bool IsFinished) {
  this->IsFinished = IsFinished;
}

void udocs_processor::FtxSimpleView::ReportSuccess(const std::string &Message) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  CurrentState = State::SUCCESS;
  StateMessage = Message;
}

void udocs_processor::FtxSimpleView::ReportError(const std::string &Message) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  CurrentState = State::FAILURE;
  StateMessage = Message;
}

void udocs_processor::FtxSimpleView::ReportWarning(const std::string &Message) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  CurrentState = State::WARNING;
  StateMessage = Message;
}

void udocs_processor::FtxSimpleView::ReportProgress(
    const std::string &Message) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  CurrentState = State::PROGRESS;
  StateMessage = Message;
}

void udocs_processor::FtxSimpleView::ShowList(
    std::vector<std::string> List) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  ListElements.clear();
  ListElements.reserve(List.size());
  for (const auto& Item : List) {
    ListElements.emplace_back(ftxui::text(Item));
  }
  ListElement = ftxui::vbox(ListElements);
  this->List = std::move(List);
  HasList = true;
}

udocs_processor::FtxSimpleView::FtxSimpleView()
  : Screen(ftxui::ScreenInteractive::TerminalOutput()) {}

void udocs_processor::FtxSimpleView::ShowDescription(
    const std::string &Description) {
  std::lock_guard<std::mutex> Lock{SelectionsProtection};

  this->Description = Description;
}

bool udocs_processor::FtxSimpleView::Confirm(const std::string &Message) {
  DoConfirm = true;
  ConfirmationMessage = fmt::format(CONFIRM_FORMAT, Message);

  std::unique_lock<std::mutex> Lock{ConfirmationMutex};
  ConfirmationCv.wait(Lock);

  DoConfirm = false;

  return ConfirmationInput == CONFIRM_YES;
}
