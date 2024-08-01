#include <gtest/gtest.h>
#include <server/command_handler.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

using hours_t = boost::posix_time::hours;
using minutes_t = boost::posix_time::minutes;

TEST(ParseTime, CorrectTime1) {
    std::string input = "14:20";
    hours_t hours_expected(14);
    minutes_t minutes_expected(20);
    std::pair<hours_t, minutes_t> expected = {hours_expected, minutes_expected};
    std::pair<hours_t, minutes_t> result = CommandHandler::parse_time(input);
    EXPECT_EQ(result, expected);
}

TEST(ParseTime, CorrectTime2) {
    std::string input = "4:20";
    hours_t hours_expected(4);
    minutes_t minutes_expected(20);
    std::pair<hours_t, minutes_t> expected = {hours_expected, minutes_expected};
    std::pair<hours_t, minutes_t> result = CommandHandler::parse_time(input);
    EXPECT_EQ(result, expected);
}

TEST(ParseTime, IncorrectTime1) {
    std::string input = "43:20";
    EXPECT_THROW(CommandHandler::parse_time(input), std::exception);
}

TEST(ParseTime, IncorrectTime2) {
    std::string input = "3:76";
    EXPECT_THROW(CommandHandler::parse_time(input), std::exception);
}

TEST(ParseTime, IncorrectTime3) {
    std::string input = "3234:764";
    EXPECT_THROW(CommandHandler::parse_time(input), std::exception);
}

TEST(ParseTime, IncorrectTime4) {
    std::string input = "-4:05";
    EXPECT_THROW(CommandHandler::parse_time(input), std::exception);
}

TEST(ParseTime, IncorrectTime5) {
    std::string input = "4:-20";
    EXPECT_THROW(CommandHandler::parse_time(input), std::exception);
}

using date_t = boost::gregorian::date;

TEST(ParseDate, CorrectDate1) {
    std::string input = "01/02/2024";
    date_t expected(2024, 2, 1);
    date_t result = CommandHandler::parse_date(input);
    EXPECT_EQ(result, expected);
}

TEST(ParseDate, CorrectDate2) {
    std::string input = "01.02.2130";
    date_t expected(2130, 2, 1);
    date_t result = CommandHandler::parse_date(input);
    EXPECT_EQ(result, expected);
}

TEST(ParseDate, CorrectDate3) {
    std::string input = "12.02.25";
    date_t expected(2025, 2, 12);
    date_t result = CommandHandler::parse_date(input);
    EXPECT_EQ(result, expected);
}

TEST(ParseDate, CorrectDateWithoutYear1) {
    std::string input = "31/01";
    date_t expected(2024, 1, 31);
    date_t result = CommandHandler::parse_date(input);
    EXPECT_EQ(result, expected);
}

TEST(ParseDate, CorrectDateWithoutYear2) {
    std::string input = "31.01";
    date_t expected(2024, 1, 31);
    date_t result = CommandHandler::parse_date(input);
    EXPECT_EQ(result, expected);
}

// February has 29 days in leap year

TEST(ParseDate, IncorrectDate1) {
    std::string input = "29.02.13";
    EXPECT_THROW(CommandHandler::parse_date(input), std::exception);
}

// April has 30 days

TEST(ParseDate, IncorrectDate2) {
    std::string input = "31.04.12";
    EXPECT_THROW(CommandHandler::parse_date(input), std::exception);
}

// Year has 12 months

TEST(ParseDate, IncorrectDateRegex3) {
    std::string input = "01.13.24";
    EXPECT_THROW(CommandHandler::parse_date(input), std::exception);
}

// Regex

TEST(ParseDate, IncorrectDateRegex1) {
    std::string input = "03245.142345.23425";
    EXPECT_THROW(CommandHandler::parse_date(input), std::exception);
}

TEST(ParseDate, IncorrectDateRegex2) {
    std::string input = "03245/5142345/23425";
    EXPECT_THROW(CommandHandler::parse_date(input), std::exception);
}
