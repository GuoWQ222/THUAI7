#include <optional>
#include <string>
#include "AI.h"
#include "API.h"
#include "utils.hpp"
#include "structures.h"

#undef GetMessage
#undef SendMessage
#undef PeekMessage

#define PI 3.14159265358979323846

ShipDebugAPI::ShipDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID) :
    logic(logic)
{
    std::string fileName = "logs/api-" + std::to_string(playerID) + "-log.txt";
    auto fileLogger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
    auto printLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::string pattern = "[api " + std::to_string(playerID) + "] [%H:%M:%S.%e] [%l] %v";
    fileLogger->set_pattern(pattern);
    printLogger->set_pattern(pattern);
    if (file)
        fileLogger->set_level(spdlog::level::trace);
    else
        fileLogger->set_level(spdlog::level::off);
    if (print)
        printLogger->set_level(spdlog::level::info);
    else
        printLogger->set_level(spdlog::level::off);
    if (warnOnly)
        printLogger->set_level(spdlog::level::warn);
    logger = std::make_unique<spdlog::logger>("apiLogger", spdlog::sinks_init_list{fileLogger, printLogger});
    logger->flush_on(spdlog::level::warn);
}

void ShipDebugAPI::StartTimer()
{
    startPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(startPoint);
    logger->info("=== AI.play() ===");
    logger->info("StartTimer: {}", std::ctime(&t));
}

void ShipDebugAPI::EndTimer()
{
    logger->info("Time elapsed: {}ms", Time::TimeSinceStart(startPoint));
}

int32_t ShipDebugAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

std::future<bool> ShipDebugAPI::SendTextMessage(int64_t toID, std::string message)
{
    logger->info("SendTextMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=, message = std::move(message)]()
                      { auto result = logic.Send(toID, std::move(message), false);
                        if (!result)
                            logger->warn("SendTextMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ShipDebugAPI::SendBinaryMessage(int64_t toID, std::string message)
{
    logger->info("SendBinaryMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=, message = std::move(message)]()
                      { auto result = logic.Send(toID, std::move(message), true);
                        if (!result)
                            logger->warn("SendBinaryMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

bool ShipDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.HaveMessage();
    if (!result)
        logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

std::pair<int64_t, std::string> ShipDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.GetMessage();
    if (result.first == -1)
        logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

bool ShipDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return false;
    else
        return logic.WaitThread();
}
std::future<bool> ShipDebugAPI::Move(int64_t timeInMilliseconds, double angleInRadian)
{
    logger->info("Move: timeInMilliseconds = {}, angleInRadian = {}, called at {}ms", timeInMilliseconds, angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Move(timeInMilliseconds, angleInRadian);
                        if (!result)
                            logger->warn("Move: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ShipDebugAPI::MoveDown(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, 0);
}

std::future<bool> ShipDebugAPI::MoveRight(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 0.5);
}

std::future<bool> ShipDebugAPI::MoveUp(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI);
}

std::future<bool> ShipDebugAPI::MoveLeft(int64_t timeInMilliseconds)
{
    return Move(timeInMilliseconds, PI * 1.5);
}

std::future<bool> ShipDebugAPI::Attack(double angleInRadian)
{
    logger->info("Attack: angleInRadian = {}, called at {}ms", angleInRadian, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=]()
                      { auto result = logic.Attack(angleInRadian);
                        if (!result)
                            logger->warn("Attack: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> ShipDebugAPI::Recover()
{
    return std::async(std::launch::async, [=]()
                      { return logic.Recover(); });
}
std::future<bool> ShipDebugAPI::Produce()
{
    return std::async(std::launch::async, [=]()
                      { return logic.Produce(); });
}
std::future<bool> ShipDebugAPI::ReBuild(THUAI7::ConstructionType constructionType)
{
    return std::async(std::launch::async, [=]()
                      { return logic.ReBuild(constructionType); });
}

std::future<bool> ShipDebugAPI::Construct(THUAI7::ConstructionType constructionType)
{
    return std::async(std::launch::async, [=]()
                      { return logic.Construct(constructionType); });
}

std::vector<std::shared_ptr<const THUAI7::Ship>> ShipDebugAPI::GetShips() const
{
    return logic.GetShips();
}

std::vector<std::shared_ptr<const THUAI7::Ship>> ShipDebugAPI::GetEnemyShips() const
{
    return logic.GetEnemyShips();
}

std::vector<std::shared_ptr<const THUAI7::Bullet>> ShipDebugAPI::GetBullets() const
{
    return logic.GetBullets();
}

std::vector<std::vector<THUAI7::PlaceType>> ShipDebugAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

THUAI7::PlaceType ShipDebugAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

int32_t ShipDebugAPI::GetConstructionHp(int32_t cellX, int32_t cellY) const
{
    return logic.GetConstructionHp(cellX, cellY);
}

int32_t ShipDebugAPI::GetWormHp(int32_t cellX, int32_t cellY) const
{
    return logic.GetWormHp(cellX, cellY);
}

int32_t ShipDebugAPI::GetResourceState(int32_t cellX, int32_t cellY) const
{
    return logic.GetResourceState(cellX, cellY);
}

int32_t ShipDebugAPI::GetHomeHp() const
{
    return logic.GetHomeHp();
}

std::shared_ptr<const THUAI7::GameInfo> ShipDebugAPI::GetGameInfo() const
{
    return logic.GetGameInfo();
}

std::vector<int64_t> ShipDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::shared_ptr<const THUAI7::Ship> ShipDebugAPI::GetSelfInfo() const
{
    return logic.ShipGetSelfInfo();
}

bool ShipDebugAPI::HaveView(int32_t gridX, int32_t gridY) const
{
    auto selfInfo = GetSelfInfo();
    return logic.HaveView(gridX, gridY, selfInfo->x, selfInfo->y, selfInfo->viewRange);
}

int32_t ShipDebugAPI::GetMoney() const
{
    return logic.GetMoney();
}

int32_t ShipDebugAPI::GetScore() const
{
    return logic.GetScore();
}

void ShipDebugAPI::Print(std::string str) const
{
    logger->info(str);
}

void ShipDebugAPI::PrintShip() const
{
    for (const auto& Ship : logic.GetShips())
    {
        logger->info("******Ship Info******");
        logger->info("type={}, playerID={}, GUID={}, x={}, y={}", THUAI7::shipTypeDict[Ship->shipType],Ship->playerID, Ship->guid, Ship->x, Ship->y);
        logger->info("state={},speed={}, view range={},facing direction={}", THUAI7::shipStateDict[Ship->shipState],Ship->speed, Ship->viewRange,Ship->facingDirection);
        logger->info("************************\n");
    }
}

void ShipDebugAPI::PrintSelfInfo() const
{
    auto Ship = logic.ShipGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("type={}, playerID={}, GUID={}, x={}, y={}", THUAI7::shipTypeDict[Ship->shipType], Ship->playerID, Ship->guid, Ship->x, Ship->y);
    logger->info("state={},speed={}, view range={},facing direction={}", THUAI7::shipStateDict[Ship->shipState], Ship->speed, Ship->viewRange, Ship->facingDirection);
    logger->info("*********************\n");
}

TeamDebugAPI::TeamDebugAPI(ILogic& logic, bool file, bool print, bool warnOnly, int64_t playerID) :
    logic(logic)
{
    std::string fileName = "logs/api-" + std::to_string(playerID) + "-log.txt";
    auto fileLogger = std::make_shared<spdlog::sinks::basic_file_sink_mt>(fileName, true);
    auto printLogger = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    std::string pattern = "[api" + std::to_string(playerID) + "] [%H:%M:%S.%e] [%l] %v";
    fileLogger->set_pattern(pattern);
    printLogger->set_pattern(pattern);
    if (file)
        fileLogger->set_level(spdlog::level::trace);
    else
        fileLogger->set_level(spdlog::level::off);
    if (print)
        printLogger->set_level(spdlog::level::info);
    else
        printLogger->set_level(spdlog::level::off);
    if (warnOnly)
        printLogger->set_level(spdlog::level::warn);
    logger = std::make_unique<spdlog::logger>("apiLogger", spdlog::sinks_init_list{fileLogger, printLogger});
}

void TeamDebugAPI::StartTimer()
{
    startPoint = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(startPoint);
    logger->info("=== AI.play() ===");
    logger->info("StartTimer: {}", std::ctime(&t));
}

void TeamDebugAPI::EndTimer()
{
    logger->info("Time elapsed: {}ms", Time::TimeSinceStart(startPoint));
}

int32_t TeamDebugAPI::GetFrameCount() const
{
    return logic.GetCounter();
}

std::future<bool> TeamDebugAPI::SendTextMessage(int64_t toID, std::string message)
{
    logger->info("SendTextMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=, message = std::move(message)]()
                      { auto result = logic.Send(toID, std::move(message), false);
                        if (!result)
                            logger->warn("SendTextMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

std::future<bool> TeamDebugAPI::SendBinaryMessage(int64_t toID, std::string message)
{
    logger->info("SendBinaryMessage: toID = {}, message = {}, called at {}ms", toID, message, Time::TimeSinceStart(startPoint));
    return std::async(std::launch::async, [=, message = std::move(message)]()
                      { auto result = logic.Send(toID, std::move(message), true);
                        if (!result)
                            logger->warn("SendBinaryMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
                        return result; });
}

bool TeamDebugAPI::HaveMessage()
{
    logger->info("HaveMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.HaveMessage();
    if (!result)
        logger->warn("HaveMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

std::pair<int64_t, std::string> TeamDebugAPI::GetMessage()
{
    logger->info("GetMessage: called at {}ms", Time::TimeSinceStart(startPoint));
    auto result = logic.GetMessage();
    if (result.first == -1)
        logger->warn("GetMessage: failed at {}ms", Time::TimeSinceStart(startPoint));
    return result;
}

bool TeamDebugAPI::Wait()
{
    logger->info("Wait: called at {}ms", Time::TimeSinceStart(startPoint));
    if (logic.GetCounter() == -1)
        return false;
    else
        return logic.WaitThread();
}

std::vector<std::shared_ptr<const THUAI7::Ship>> TeamDebugAPI::GetShips() const
{
    return logic.GetShips();
}

std::vector<std::shared_ptr<const THUAI7::Ship>> TeamDebugAPI::GetEnemyShips() const
{
    return logic.GetEnemyShips();
}

std::vector<std::shared_ptr<const THUAI7::Bullet>> TeamDebugAPI::GetBullets() const
{
    return logic.GetBullets();
}

std::vector<std::vector<THUAI7::PlaceType>> TeamDebugAPI::GetFullMap() const
{
    return logic.GetFullMap();
}

THUAI7::PlaceType TeamDebugAPI::GetPlaceType(int32_t cellX, int32_t cellY) const
{
    return logic.GetPlaceType(cellX, cellY);
}

int32_t TeamDebugAPI::GetConstructionHp(int32_t cellX, int32_t cellY) const
{
    return logic.GetConstructionHp(cellX, cellY);
}

int32_t TeamDebugAPI::GetWormHp(int32_t cellX, int32_t cellY) const
{
    return logic.GetWormHp(cellX, cellY);
}

int32_t TeamDebugAPI::GetResourceState(int32_t cellX, int32_t cellY) const
{
    return logic.GetResourceState(cellX, cellY);
}

int32_t TeamDebugAPI::GetHomeHp() const
{
    return logic.GetHomeHp();
}

std::shared_ptr<const THUAI7::GameInfo> TeamDebugAPI::GetGameInfo() const
{
    return logic.GetGameInfo();
}

std::vector<int64_t> TeamDebugAPI::GetPlayerGUIDs() const
{
    return logic.GetPlayerGUIDs();
}

std::shared_ptr<const THUAI7::Team> TeamDebugAPI::GetSelfInfo() const
{
    return logic.TeamGetSelfInfo();
}

int32_t TeamDebugAPI::GetMoney() const
{
    return logic.GetMoney();
}

int32_t TeamDebugAPI::GetScore() const
{
    return logic.GetScore();
}

std::future<bool> TeamDebugAPI::InstallModule(int64_t playerID, THUAI7::ModuleType moduleType)
{
    return std::async(std::launch::async, [=]()
                      { return logic.InstallModule(playerID, moduleType);});
}
void TeamDebugAPI::PrintSelfInfo() const
{
    auto Team = logic.TeamGetSelfInfo();
    logger->info("******Self Info******");
    logger->info("playerID={}, teamID={}, score={}, money={}", Team->playerID, Team->teamID, Team->score, Team->money);
    logger->info("*********************\n");
}

void ShipDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}

void TeamDebugAPI::Play(IAI& ai)
{
    ai.play(*this);
}
