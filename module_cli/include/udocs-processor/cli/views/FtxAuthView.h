/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <vector>
#include <map>
#include <string>
#include "AuthView.h"
#include "FtxSimpleView.h"

namespace udocs_processor {
class FtxAuthView : public virtual AuthView, public virtual FtxSimpleView {
 public:
  FtxAuthView();

 private:
  bool DoExit_ = false;
  bool HasFinished = false;
};
}  // namespace udocs_processor
