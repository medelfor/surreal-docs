/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <mutex> NOLINT()
#include <vector>
#include <string>
#include "GenerateView.h"

namespace udocs_processor {
class NonInteractiveGenerateView : public GenerateView {
 public:
  explicit NonInteractiveGenerateView(std::ostream &OutputStream);

  void Init() override;

  void SetStatus(Status Status_, const std::string &Message) override;

  void SetFinished(bool HasFinished) override;

  bool DoExit() const override;

  void Tick() override;

  void Destroy() override;

  ~NonInteractiveGenerateView() override = default;

 private:
  std::mutex ToRenderProtection;
  std::vector<std::string> ToRender;
  bool HasFinished = false;
  bool HasErrored = false;
  std::ostream& OutputStream;
  float Progress = 0.0f;
};
}  // namespace udocs_processor
