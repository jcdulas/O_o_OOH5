#pragma once

#include "oor/api/Deck.hpp"

namespace oor::formats::ooh5
{
[[nodiscard]] std::shared_ptr<api::Deck> read(const std::string& path, const api::DeckReadSettings& settings = {});
bool recognizes(const std::string& path);
results::ResultWriteReport write(const api::Deck& deck, const std::string& path,
                                 const results::ArchiveWriteOptions& options = {});
} // namespace oor::formats::ooh5
