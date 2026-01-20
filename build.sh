mkdir -p build
pushd build

rm -rf ./*
emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DENABLE_SDL=OFF \
    -DENABLE_DECODER=OFF \
    `# Bypass x86 check` \
    -DCMAKE_REQUIRED_DEFINITIONS=-D__x86_64 \
    `# Enable SIMD` \
    -DCMAKE_REQUIRED_FLAGS="-msimd128" \
    -DCMAKE_CXX_FLAGS="-msimd128 -flto"
emmake make -j"$(( $(nproc) - 1 ))"

mkdir -p wasm
emcc -o wasm/libde265.mjs \
    -flto \
    -O3 \
    `# Some methods requires more stack than default 64k` \
    -sSTACK_SIZE=128000 \
    -sALLOW_MEMORY_GROWTH \
    -sENVIRONMENT=worker \
    -sMODULARIZE \
    -sNO_DYNAMIC_EXECUTION \
    -sWASM_BIGINT \
    `# Slightly reduce size` \
    -sEVAL_CTORS=2 \
    -lembind \
    -sEMBIND_AOT \
    --emit-tsd \
    libde265.d.ts \
    -I.. \
    -I. \
    ../embind.cpp \
    libde265/libde265.a

cp ../package.json ../README.md wasm

popd
