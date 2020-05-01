#include <catch.hpp>

#include <mw/core/mmr/backends/FileBackend.h>
#include <mw/core/models/tx/IKernel.h>
#include <mw/core/crypto/Random.h>
#include <mw/core/file/FileRemover.h>

using namespace mmr;

static FilePath CreateTempDir()
{
    return FilePath(fs::temp_directory_path() / (StringUtil::ToWide(Random::CSPRNG<6>().GetBigInt().ToHex()) + L"\u30c4"));
}

TEST_CASE("mmr::FileBackend")
{
    FilePath tempDir = CreateTempDir();
    FileRemover remover(tempDir);

    {
        auto pBackend = FileBackend::Open(tempDir, tl::nullopt);
        pBackend->AddLeaf(mmr::Leaf::Create(mmr::LeafIndex::At(0), { 0x05, 0x03, 0x07 }));
        pBackend->Commit();
    }
    {
        auto pBackend = FileBackend::Open(tempDir, tl::nullopt);
        REQUIRE(pBackend->GetNumLeaves() == 1);
    }
}