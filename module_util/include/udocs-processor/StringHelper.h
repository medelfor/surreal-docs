/* Copyright © 2022, Medelfor, Limited. All rights reserved. */

#pragma once

#include <string>
#include <functional>

namespace udocs_processor {
class StringHelper {
 public:
  static std::string EscapeHtml(const std::string& Input,
      bool AllowWbr = false);

  static std::string EscapeUrl(const std::string& Input,
      bool DoubleEscape = false);

  static bool IsAlpha(char c);

  static bool IsNumeric(char c);

  static bool IsAlphaNumeric(char c);

  static bool IsId(char c);

  static std::string& TrimAllWhitespaces(std::string& Input);

  static void ReplaceAll(std::string& Haystack, const std::string& Needle,
      const std::string& Replacement);

  static std::string ToLower(const std::string &Input);

  static std::string MakeId(const std::string& Text, bool Lowercase = true);

  static std::string MakeMdLink(
      const std::string &Text, const std::string &AltText,
      const std::string &Url);

  static std::string RemoveUnnecessarySpaces(const std::string& Input);

  static std::string ws2s(const std::wstring& Wstr);

  using TagHandlerType =
      std::function<std::string(const std::string&, const std::string&)>;
  /*
   * Part1: <tag id="
   * Part2: ">
   * Part3: </tag>
   *
   * Handler 1st param: everything between Part1 and Part2
   * Handler 2nd param: everything between Part2 and Part3
   * */
  static std::string ProcessTags(const std::string& Text,
      const std::string& Part1, const std::string& Part2,
      const std::string& Part3, TagHandlerType Handler);

  static std::string ConcatDescriptions(const std::string& Description1,
      const std::string& Description2);

  static std::string FormatByCapital(const std::string_view& Name,
      const std::string& Breaker);

  static std::string Normalize(const std::string& Path);

  static std::string ConvertDocPathToLocalRepresentation(
      const std::string& Page);

  static std::string ConvertImagePathToLocalRepresentation(
      const std::string& Image);

  static std::string Embrace(const std::string& String);

  static float SafeStof(const std::string& Value) {
    try {
      return std::stof(Value);
    } catch (const std::exception& Exc) {
      return 0;
    }
  }

  static uint64_t SafeStoull(const std::string& Value) {
    try {
      return std::stoull(Value);
    } catch (const std::exception& Exc) {
      return 0;
    }
  }

  static int64_t SafeStoll(const std::string& Value) {
    try {
      return std::stoll(Value);
    } catch (const std::exception& Exc) {
      return 0;
    }
  }

  static double SafeStod(const std::string& Value) {
    try {
      return std::stod(Value);
    } catch (const std::exception& Exc) {
      return 0;
    }
  }

  constexpr static char REPLACEMENT_HTML_CODE[] = "&#xfffd;";
  constexpr static char SPACE_CH = ' ';

 private:
  static constexpr char DIRECTORY_SEPARATOR = '\\';
  static constexpr char HTML_EXTENSION[] = ".html";
  constexpr static char URL_SEPARATOR[] = "/";
  constexpr static char URL_SEPARATOR_LOCAL[] = "+";

  inline static bool CheckIfWbr(std::string_view String, size_t Pos);

  constexpr static char MD_LINK_TEXT_START[] = "[";
  constexpr static char MD_LINK_TEXT_END[] = "]";
  constexpr static char MD_LINK_URL_START[] = "(";
  constexpr static char MD_LINK_URL_END[] = ")";
  constexpr static char MD_LINK_TITLE_START[] = "\"";
  constexpr static char MD_LINK_TITLE_END[] = "\"";

  constexpr static char ALPHABET_LOWER_START = 'a';
  constexpr static char ALPHABET_LOWER_END = 'z';
  constexpr static char ALPHABET_UPPER_START = 'A';
  constexpr static char ALPHABET_UPPER_END = 'Z';
  constexpr static char DIGITS_START = '0';
  constexpr static char DIGITS_END = '9';

  constexpr static char AMPERSAND = '&';
  constexpr static char PERCENT = '%';
  constexpr static char AMPERSAND_HTML_CODE[] = "&amp;";
  constexpr static char ENCODED_PERCENT[] = "%25";
  constexpr static char LESS = '<';
  constexpr static char LESS_HTML_CODE[] = "&lt;";
  constexpr static char MORE = '>';
  constexpr static char MORE_HTML_CODE[] = "&gt;";
  constexpr static char DOUBLE_QUOTE = '"';
  constexpr static char DOUBLE_QUOTE_HTML_CODE[] = "&quot;";
  constexpr static char FULL_WBR[] = "<wbr/>";

  constexpr static size_t ESCAPED_URL_SYMBOL_SIZE = 3;
  constexpr static size_t DOUBLE_ESCAPED_URL_SYMBOL_SIZE = 4;
  constexpr static int URL_STRING_RESERVE = 64;
  constexpr static int HTML_STRING_RESERVE = 64;

  constexpr static char WHITESPACES[] = " \n\t\r";
  constexpr static char DESCRIPTION_CONCAT_GLUE[] = "\n\n";
  constexpr static char SPACE = ' ';
  constexpr static char SPACE_STR[] = " ";
  constexpr static char SPACE_ESCAPED[] = "` ";
  constexpr static char UNDERSCORE = '_';

  constexpr static int AVERAGE_WORD_IN_NAME = 6;
  constexpr static int ASCII_MIN = 0;
  constexpr static int ASCII_MAX = 255;

  constexpr static char CODE_SEPARATION_PATTERN[] =
      R"(([a-z0-9_>])\s+([<a-z0-9_]))";
  constexpr static char CODE_SEPARATION_REPLACE_PATTERN[] = R"($1#$2)";
  constexpr static char CODE_SEPARATION_REPLACE_CHAR[] = "#";
  constexpr static char CODE_ADD_SPACE_TO_THE_RIGHT[] = R"((\*|,|&&|&))";
  constexpr static char CODE_ADD_SPACE_TO_THE_RIGHT_REPLACE[] = R"($1 )";

  constexpr static int ID_STRING_RESERVE = 64;
  constexpr static char ID_REPLACEMENT_OF_UNSUPPORTED_SYMBOLS[] = "-";

  constexpr static char URL_UNESCAPED[] = "~-_.+!*(),%#@?=;:/,+$&";
  constexpr static char HEX[] = "0123456789ABCDEF";
};
}  // namespace udocs_processor
