# AssetHelpers.cmake
#
# 此文件已迁移至 software/game-core/cmake/AssetHelpers.cmake。
# 保留此文件以兼容旧引用，实际逻辑已转发到新位置。
#
# 旧用法:
#   set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
#   include(${REPO_ROOT}/cmake/AssetHelpers.cmake)
#
# 新用法:
#   set(GAME_CORE_ROOT "<path-to>/software/game-core")
#   include(${GAME_CORE_ROOT}/cmake/AssetHelpers.cmake)

message(AUTHOR_WARNING
	"cmake/AssetHelpers.cmake is deprecated. "
	"Use software/game-core/cmake/AssetHelpers.cmake instead."
)

include(${CMAKE_CURRENT_LIST_DIR}/../software/game-core/cmake/AssetHelpers.cmake)
