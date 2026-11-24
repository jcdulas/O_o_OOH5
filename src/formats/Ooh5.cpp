#include "oor/formats/ooh5.hpp"

#include "oor/api/Results.hpp"
#include "oor/base/text/TextUtilities.hpp"
#include "oor/results/archive/ArchiveModel.hpp"
#include "oor/results/archive/ArchiveResultFile.hpp"
#include "oor/results/archive/ArchiveWriter.hpp"
#include "oor/results/core/ReadResults.hpp"

namespace oor::formats::ooh5
{
using api::Deck;
using api::DeckReadSettings;

std::shared_ptr<Deck> read(const std::string& path, const DeckReadSettings& settings)
{
    model::ModelReadOptions options;
    options.strict = settings.strict;
    return Deck::fromModel(results::archive::readModel(base::pathFromUtf8(path), options));
}

bool recognizes(const std::string& path)
{
    return results::ArchiveResultFile::isArchive(base::pathFromUtf8(path));
}

results::ResultWriteReport write(const Deck& deck, const std::string& path, const results::ArchiveWriteOptions& options)
{
    auto settings = options;
    settings.model = deck.model();
    return results::writeArchive(base::pathFromUtf8(path), results::ResultTables{}, settings);
}

} // namespace oor::formats::ooh5

namespace oor::api
{
std::shared_ptr<Results> Results::open(const std::string& path)
{
    auto file = results::readResults(path);
    auto archive = std::dynamic_pointer_cast<results::ArchiveResultFile>(file);
    if (archive && archive->hasModel())
        return fromFile(std::move(file), [archive] { return archive->readModel(); });
    return fromFile(std::move(file));
}

results::ResultWriteReport Results::writeArchive(const std::string& path,
                                                 const results::ArchiveWriteOptions& options) const
{
    auto settings = options;
    if (!settings.model)
    {
        if (m_deck)
            settings.model = m_deck->model();
        else if (m_embeddedModel)
            settings.model = m_embeddedModel();
    }
    return results::writeArchive(base::pathFromUtf8(path), *m_tables, settings);
}

} // namespace oor::api
