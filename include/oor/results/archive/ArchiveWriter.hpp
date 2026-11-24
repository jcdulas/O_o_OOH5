#pragma once

#include "oor/model/core/Model.hpp"
#include "oor/results/core/ResultTables.hpp"
#include "oor/results/writers/ResultWriteReport.hpp"
#include "oor/results/writers/ArchiveWriteOptions.hpp"
#include "oor/results/writers/ResultWriteSession.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace oor::results
{

// Writes an archive by batches: `beginArchive`, then `push` tables and `pushNamed` named results as many times as
// needed, then `finish`. Each push writes its tables at once to a temporary file next to the target and keeps nothing
// of their values; `finish` writes the index and puts the file in place. `abort` (or the destruction of an unfinished
// session) removes the temporary file, so a failure never leaves a half-written archive at the target.
[[nodiscard]] std::unique_ptr<ResultWriteSession> beginArchive(const std::filesystem::path& target,
                                                               const ArchiveWriteOptions& options = {});

// Writes every table and named result of `tables` to an archive in one call.
ResultWriteReport writeArchive(const std::filesystem::path& target, const ResultTables& tables,
                               const ArchiveWriteOptions& options = {});

} // namespace oor::results
