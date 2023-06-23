/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#include "udocs-processor/StringHelper.h"

#define NOLINT()

#include <regex> NOLINT()
#include <algorithm>
#include <locale>
#include <codecvt>

// AllowWbr is deprecated
std::string udocs_processor::StringHelper::EscapeHtml(
        const std::string &Input, bool AllowWbr) {
  std::string Result;
  Result.reserve(HTML_STRING_RESERVE);

  size_t ToFlush = 0;
  const char* Start = Input.c_str();
  size_t Size = Input.size();

  for (size_t p = 0; p < Size; ) {
    bool DoFlush = false;
    std::string ToRender;
    size_t NewP = p + 1;
    // to speed up escaping
    switch (Input[p]) {
      case AMPERSAND:
        ToRender = AMPERSAND_HTML_CODE;
        DoFlush = true;
        break;
      case LESS:
        if (AllowWbr && CheckIfWbr(Input, p)) {
          ToRender = FULL_WBR;
          NewP = p + strlen(FULL_WBR);
        } else {
          ToRender = LESS_HTML_CODE;
        }
        DoFlush = true;
        break;
      case MORE:
        ToRender = MORE_HTML_CODE;
        DoFlush = true;
        break;
      case DOUBLE_QUOTE:
        ToRender = DOUBLE_QUOTE_HTML_CODE;
        DoFlush = true;
        break;
      default:
        ++ToFlush;
    }

    if (DoFlush) {
      Result += std::string(Start + p - ToFlush, ToFlush);
      ToFlush = 0;
    }

    Result += ToRender;
    p = NewP;
  }

  if (ToFlush > 0) {
    Result += std::string(Start + Size - ToFlush, ToFlush);
  }

  return Result;
}

inline bool udocs_processor::StringHelper::CheckIfWbr(
    std::string_view String, size_t Pos) {
  static const size_t WBR_LEN = strlen(FULL_WBR);

  for (size_t i = 0; i < WBR_LEN; ++i) {
    if (String[i + Pos] != FULL_WBR[i]) {
      return false;
    }
  }

  return true;
}

std::string udocs_processor::StringHelper::EscapeUrl(
    const std::string& Url, bool DoubleEscape) {
  std::string Result;
  Result.reserve(URL_STRING_RESERVE);

  const char* Data = Url.c_str();
  size_t Size = Url.size();
  size_t Begin = 0;
  size_t Off = 0;

  while (true) {
    while (Off < Size &&
        (!(DoubleEscape && Data[Off] == PERCENT) &&
         (strchr(URL_UNESCAPED, Data[Off]) || IsAlphaNumeric(Data[Off]))))
      Off++;
    if (Off > Begin)
      Result += {Data + Begin, Off - Begin};

    if (Off < Size) {
      char Hex[std::max(
          ESCAPED_URL_SYMBOL_SIZE, DOUBLE_ESCAPED_URL_SYMBOL_SIZE)];

      switch (Data[Off]) {
        default: {
          size_t Ind = 0;
          Hex[Ind++] = '%';
          Hex[Ind++] = HEX[(static_cast<unsigned>(Data[Off]) >> 4) & 0xf];
          Hex[Ind++] = HEX[(static_cast<unsigned>(Data[Off]) >> 0) & 0xf];
          // if (DoubleEscape) Result += ENCODED_PERCENT;
          Result += {Hex, Ind};
          break;
        }
      }
      Off++;
    } else {
      break;
    }

    Begin = Off;
  }

  return Result;
}

bool udocs_processor::StringHelper::IsAlpha(char c) {
  return (c >= ALPHABET_LOWER_START && c <= ALPHABET_LOWER_END) ||
      (c >= ALPHABET_UPPER_START && c <= ALPHABET_UPPER_END);
}

bool udocs_processor::StringHelper::IsNumeric(char c) {
  return c >= DIGITS_START && c <= DIGITS_END;
}

bool udocs_processor::StringHelper::IsAlphaNumeric(char c) {
  return IsAlpha(c) || IsNumeric(c);
}

bool udocs_processor::StringHelper::IsId(char c) {
  return IsAlpha(c) || IsNumeric(c) || c == UNDERSCORE;
}

std::string &
    udocs_processor::StringHelper::TrimAllWhitespaces(std::string &Input) {
  Input.erase(0, Input.find_first_not_of(WHITESPACES));
  Input.erase(Input.find_last_not_of(WHITESPACES) + 1);

  return Input;
}

void udocs_processor::StringHelper::ReplaceAll(
    std::string& Haystack, const std::string& Needle,
    const std::string& Replacement) {
  size_t Pos = 0;
  while ((Pos = Haystack.find(Needle, Pos)) != std::string::npos) {
    Haystack.replace(Pos, Needle.length(), Replacement);
    Pos += Replacement.length();
  }
}

std::string udocs_processor::StringHelper::ToLower(const std::string &Input) {
  std::string Result(Input.size(), SPACE);
  std::transform(Input.begin(), Input.end(),
      Result.begin(), ::tolower);

  return Result;
}

std::string udocs_processor::StringHelper::MakeId(
    const std::string& Text, bool Lowercase) {
  std::string Id;
  Id.reserve(ID_STRING_RESERVE);

  bool DoInsertSpace = false;
  bool DoReplaceUnsupportedSymbols = false;
  for (size_t i = 0; i < Text.size(); ++i) {
    char c = Text[i];

    if (StringHelper::IsAlphaNumeric(c)) {
      // take unsupported symbols into account only after first supported symbol
      DoReplaceUnsupportedSymbols = true;
      if (DoInsertSpace) {
        // done, so several unsupported symbols would collapse into one dash
        Id += ID_REPLACEMENT_OF_UNSUPPORTED_SYMBOLS;
        DoInsertSpace = false;
      }
      Id += Lowercase ? static_cast<char>(::tolower(c)) : c;
    } else if (DoReplaceUnsupportedSymbols) {
      DoInsertSpace = true;
    }
  }

  return Id;
}

std::string udocs_processor::StringHelper::MakeMdLink(
    const std::string &Text, const std::string &AltText,
    const std::string &Url) {
  return MD_LINK_TEXT_START + Text + MD_LINK_TEXT_END +
      MD_LINK_URL_START + Url + SPACE + MD_LINK_TITLE_START +
      AltText + MD_LINK_TITLE_END + MD_LINK_URL_END;
}

// todo(artsiom.drapun) probably highly inefficient
std::string udocs_processor::StringHelper::RemoveUnnecessarySpaces(
    const std::string& Input) {
  static std::regex Separation(CODE_SEPARATION_PATTERN, std::regex::icase);
  // replace spaces on # in situations where spaces connect two keywords
  // or identifiers like: typename ClassX -> typename#ClassX
  std::string Result = std::regex_replace(Input, Separation,
      CODE_SEPARATION_REPLACE_PATTERN);

  // remove any other spaces
  ReplaceAll(Result, SPACE_STR, "");
  ReplaceAll(Result, CODE_SEPARATION_REPLACE_CHAR, SPACE_STR);

  return Result;
}

std::string udocs_processor::StringHelper::ws2s(const std::wstring& Wstr) {
  using ConvertTypeX = std::codecvt_utf8<wchar_t>;
  std::wstring_convert<ConvertTypeX, wchar_t> ConverterX;

  return ConverterX.to_bytes(Wstr);
}

std::string udocs_processor::StringHelper::ProcessTags(const std::string& Text,
      const std::string& Part1, const std::string& Part2,
      const std::string& Part3, std::function<std::string(const std::string&,
          const std::string&)> Handler) {
  if (Text.empty()) {
    return "";
  }

  std::string Result;

  size_t Part1Len = Part1.size();
  size_t Part2Len = Part2.size();
  size_t Part3Len = Part3.size();

  size_t Pos = -1;  // know it's unsigned
  size_t CopyFrom = 0;

  while ((Pos = Text.find(Part1, Pos + 1)) != std::string::npos) {
    size_t Part1EndPlus1 = Pos + Part1Len;

    if (Part1EndPlus1 < Text.size()) {
      size_t Part2Begin = Text.find(Part2, Part1EndPlus1);

      if (Part2Begin != std::string::npos) {
        size_t Part2EndPlus1 = Part2Begin + Part2Len;

        if (Part2EndPlus1 < Text.size()) {
          size_t Part3Begin = Text.find(Part3, Part2EndPlus1);

          if (Part3Begin != std::string::npos) {
            // everything's valid, let's call handler
            std::string Between1And2 = Text.substr(Part1EndPlus1,
                Part2Begin - Part1EndPlus1);
            std::string Between2And3 = Text.substr(Part2EndPlus1,
                Part3Begin - Part2EndPlus1);

            Result += Text.substr(CopyFrom, Pos - CopyFrom);
            Result += Handler(Between1And2, Between2And3);

            if (Part3Begin + Part3Len < Text.size()) {
              CopyFrom = Part3Begin + Part3Len;
            } else {
              CopyFrom = std::string::npos;
              break;
            }

            continue;
          }
        }
      }
    }

    // because there's no more valid tags
    break;
  }

  if (CopyFrom != std::string::npos) {
    Result += Text.substr(CopyFrom);
  }

  return Result;
}

std::string udocs_processor::StringHelper::ConcatDescriptions(
    const std::string &Description1, const std::string &Description2) {
  return Description1 + (!Description1.empty() && !Description2.empty()
    ? DESCRIPTION_CONCAT_GLUE : "") + Description2;
}

std::string udocs_processor::StringHelper::FormatByCapital(
    const std::string_view& Name, const std::string& Breaker) {
  std::string Result;
  size_t Size = Name.size();
  Result.reserve(Size + Size / AVERAGE_WORD_IN_NAME * Breaker.size());

  for (size_t i = 0; i < Size; ++i) {
    char c = Name[i];
    if (c >= ASCII_MIN && c <= ASCII_MAX && isupper(c) && i != 0 &&
        Name[i - 1] != SPACE_CH) {
      Result += Breaker;
    }

    Result += c;
  }

  return Result;
}

std::string udocs_processor::StringHelper::Normalize(
    const std::string& Path) {
  if (!Path.empty() && Path[Path.size() - 1] != DIRECTORY_SEPARATOR) {
    return Path + DIRECTORY_SEPARATOR;
  }

  return Path;
}

std::string udocs_processor::StringHelper::ConvertDocPathToLocalRepresentation(
    const std::string &Page) {
  std::string EscapedPage = StringHelper::EscapeUrl(Page, true);
  std::string NewUrl;
  NewUrl.reserve(EscapedPage.size() + sizeof(HTML_EXTENSION));
  NewUrl += EscapedPage;
  StringHelper::ReplaceAll(NewUrl, URL_SEPARATOR, URL_SEPARATOR_LOCAL);
  NewUrl += HTML_EXTENSION;

  return NewUrl;
}

std::string
    udocs_processor::StringHelper::ConvertImagePathToLocalRepresentation(
        const std::string& Image) {
  return StringHelper::EscapeUrl(Image, true);
}

std::string udocs_processor::StringHelper::Embrace(const std::string &String) {
  std::string Result = String;
  ReplaceAll(Result, SPACE_STR, "^ ");
  return String;
}
