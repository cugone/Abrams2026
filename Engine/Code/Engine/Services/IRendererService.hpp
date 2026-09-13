#pragma once

#include "Engine/Services/IService.hpp"

#include "Engine/Core/Gif.hpp"
#include "Engine/Core/IFont.hpp"

#include "Engine/Renderer/AnimatedSprite.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Renderer/Flipbook.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/RasterState.hpp"
#include "Engine/Renderer/Vertex3D.hpp"
#include "Engine/Renderer/Vertex3DInstanced.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/VertexCircleBuffer.hpp"
#include "Engine/Renderer/VertexBufferInstanced.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Renderer/RenderTargetStack.hpp"
#include "Engine/Renderer/RendererTypes.hpp"
#include "Engine/Renderer/Texture.hpp"

#include <memory>
#include <string>

class Rgba;
class Frustum;
class Camera2D;
class SpriteSheet;
class FrameBuffer;

class RHIDeviceContext;
class RHIDevice;
class RHIOutput;
class RHIInstance;

class DepthStencilState;

struct AnimatedSpriteDesc;
struct DepthStencilDesc;
struct light_t;
struct PointLightDesc;
struct DirectionalLightDesc;
struct SpotLightDesc;
struct ShaderProgramDesc;
struct RasterDesc;
struct SamplerDesc;
struct ComputeJob;

class IRendererService : public IService {
public:
    virtual ~IRendererService() noexcept {}

    virtual void BeginRender(Texture* color_target = nullptr, const Rgba& clear_color = Rgba::Black, Texture* depthstencil_target = nullptr) noexcept = 0;
    virtual void BeginRenderToBackbuffer(const Rgba& clear_color = Rgba::Black) noexcept = 0;
    virtual void BeginHUDRender(Camera2D& ui_camera, const Vector2& camera_position, float window_height) noexcept = 0;

    [[nodiscard]] virtual TimeUtils::FPSeconds GetGameFrameTime() const noexcept = 0;
    [[nodiscard]] virtual TimeUtils::FPSeconds GetSystemFrameTime() const noexcept = 0;
    [[nodiscard]] virtual TimeUtils::FPSeconds GetGameTime() const noexcept = 0;
    [[nodiscard]] virtual TimeUtils::FPSeconds GetSystemTime() const noexcept = 0;

    virtual void SetFullscreen(bool isFullscreen) noexcept = 0;
    virtual void SetFullscreenMode() noexcept = 0;
    virtual void SetWindowedMode() noexcept = 0;
    virtual void SetWindowTitle(const std::string& newTitle) const noexcept = 0;
    [[nodiscard]] virtual std::string GetWindowTitle() const noexcept = 0;
    virtual void SetWindowIcon(void* iconResource) noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<VertexBuffer> CreateVertexBuffer(const VertexBuffer::buffer_t& vbo) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<VertexCircleBuffer> CreateVertexCircleBuffer(const VertexCircleBuffer::buffer_t& vbco) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<VertexBufferInstanced> CreateVertexBufferInstanced(const VertexBufferInstanced::buffer_t& vbio) const noexcept =0;
    [[nodiscard]] virtual std::unique_ptr<IndexBuffer> CreateIndexBuffer(const IndexBuffer::buffer_t& ibo) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<ConstantBuffer> CreateConstantBuffer(void* const& buffer, const std::size_t& buffer_size) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<StructuredBuffer> CreateStructuredBuffer(const StructuredBuffer::buffer_t& sbo, std::size_t element_size, std::size_t element_count) const noexcept = 0;

    [[nodiscard]] virtual Texture* CreateOrGetTexture(const std::filesystem::path& filepath, const IntVector3& dimensions) noexcept = 0;
    virtual void RegisterTexturesFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept = 0;
    [[nodiscard]] virtual bool RegisterTexture(const std::string& name, std::unique_ptr<Texture> texture) noexcept = 0;
    virtual void SetTexture(Texture* texture, unsigned int registerIndex = 0) noexcept = 0;

    [[nodiscard]] virtual Texture* GetTexture(const std::string& nameOrFile) noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<Texture> CreateDepthStencil(const RHIDevice& owner, uint32_t width, uint32_t height) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> CreateDepthStencil(const RHIDevice& owner, const IntVector2& dimensions) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> CreateRenderableDepthStencil(const RHIDevice& owner, const IntVector2& dimensions) noexcept = 0;

    [[nodiscard]] virtual Texture* GetDefaultDepthStencil() const noexcept = 0;
    virtual void SetDepthStencilState(DepthStencilState* depthstencil) noexcept = 0;
    [[nodiscard]] virtual DepthStencilState* GetDepthStencilState(const std::string& name) noexcept = 0;
    virtual void CreateAndRegisterDepthStencilStateFromDepthStencilDescription(const std::string& name, const DepthStencilDesc& desc) noexcept = 0;
    virtual void EnableDepth(bool isDepthEnabled) noexcept = 0;
    virtual void EnableDepth() noexcept = 0;
    virtual void DisableDepth() noexcept = 0;
    virtual void EnableDepthWrite(bool isDepthWriteEnabled) noexcept = 0;
    virtual void EnableDepthWrite() noexcept = 0;
    virtual void DisableDepthWrite() noexcept = 0;

    virtual void SetDepthComparison(ComparisonFunction cf) noexcept = 0;
    [[nodiscard]] virtual ComparisonFunction GetDepthComparison() const noexcept = 0;
    virtual void SetStencilFrontComparison(ComparisonFunction cf) noexcept = 0;
    virtual void SetStencilBackComparison(ComparisonFunction cf) noexcept = 0;
    virtual void EnableStencilWrite() noexcept = 0;
    virtual void DisableStencilWrite() noexcept = 0;

    [[nodiscard]] virtual Texture* Create1DTexture(std::filesystem::path filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create1DTextureFromMemory(const unsigned char* data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create1DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;
    [[nodiscard]] virtual Texture* Create2DTexture(std::filesystem::path filepath, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create2DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create2DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create2DTextureFromMemory(const void* data, std::size_t elementSize, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create2DTextureArrayFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create2DTextureArrayFromFolder(const std::filesystem::path folderpath) noexcept = 0;
    [[nodiscard]] virtual Texture* Create3DTexture(std::filesystem::path filepath, const IntVector3& dimensions, const BufferUsage& bufferUsage, const BufferBindUsage& bindUsage, const ImageFormat& imageFormat) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create3DTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> Create3DTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;
    [[nodiscard]] virtual Texture* CreateTexture(std::filesystem::path filepath, const IntVector3& dimensions = IntVector3::XY_Axis, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;

    [[nodiscard]] virtual std::unique_ptr<Texture> CreateVideoTextureFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::Ayuv, const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> CreateVideoTextureFromMemory(const std::vector<Rgba>& data, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::Ayuv, const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> CreateVideoTextureFromMemory(const void* data, std::size_t elementSize, unsigned int width = 1, unsigned int height = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::Ayuv, const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Texture> CreateVideoTextureArrayFromMemory(const unsigned char* data, unsigned int width = 1, unsigned int height = 1, unsigned int depth = 1, const BufferUsage& bufferUsage = BufferUsage::Static, const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, const ImageFormat& imageFormat = ImageFormat::Ayuv, const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) noexcept = 0;

    [[nodiscard]] virtual std::shared_ptr<SpriteSheet> CreateSpriteSheet(const std::filesystem::path& filepath, unsigned int width = 1, unsigned int height = 1) noexcept = 0;
    [[nodiscard]] virtual std::shared_ptr<SpriteSheet> CreateSpriteSheet(const XMLElement& elem) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(std::filesystem::path filepath) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(const AnimatedSpriteDesc& desc) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(std::shared_ptr<SpriteSheet> sheet, const IntVector2& startSpriteCoords = IntVector2::Zero) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(std::shared_ptr<SpriteSheet> sheet, const XMLElement& elem) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<AnimatedSprite> CreateAnimatedSprite(const XMLElement& elem) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Flipbook> CreateFlipbookFromFolder(std::filesystem::path folderpath, TimeUtils::FPSeconds secondsPerFrame) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<Flipbook> CreateFlipbookFromGif([[maybe_unused]] const FileUtils::Gif& gif) noexcept = 0;

    virtual void ClearRenderTargets(const RenderTargetType& rtt) noexcept = 0;
    virtual void SetRenderTarget(FrameBuffer& frameBuffer) noexcept = 0;
    virtual void SetRenderTarget(Texture* color_target = nullptr, Texture* depthstencil_target = nullptr) noexcept = 0;
    virtual void SetRenderTargetsToBackBuffer() noexcept = 0;
    [[nodiscard]] virtual ViewportDesc GetCurrentViewport() const = 0;
    [[nodiscard]] virtual float GetCurrentViewportAspectRatio() const = 0;
    [[nodiscard]] virtual ViewportDesc GetViewport(std::size_t index) const noexcept = 0;
    [[nodiscard]] virtual unsigned int GetViewportCount() const noexcept = 0;
    [[nodiscard]] virtual std::vector<ViewportDesc> GetAllViewports() const noexcept = 0;
    virtual void SetViewport(const ViewportDesc& desc) noexcept = 0;
    virtual void SetViewport(float x, float y, float width, float height) noexcept = 0;
    virtual void SetViewport(const AABB2& viewport) noexcept = 0;
    virtual void SetViewportAndScissor(float x, float y, float width, float height) noexcept = 0;
    virtual void SetViewportAndScissor(const AABB2& viewport_and_scissor) noexcept = 0;
    virtual void SetViewports(const std::vector<AABB3>& viewports) noexcept = 0;
    virtual void SetViewportAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept = 0;
    virtual void SetViewportAndScissorAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept = 0;

    virtual void EnableScissorTest() = 0;
    virtual void DisableScissorTest() = 0;

    virtual void SetScissor(unsigned int x, unsigned int y, unsigned int width, unsigned int height) noexcept = 0;
    virtual void SetScissor(const AABB2& scissor) noexcept = 0;
    virtual void SetScissorAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept = 0;
    virtual void SetScissorAndViewport(float x, float y, float width, float height) noexcept = 0;
    virtual void SetScissorAndViewport(const AABB2& scissor_and_viewport) noexcept = 0;
    virtual void SetScissorAndViewportAsPercent(float x = 0.0f, float y = 0.0f, float w = 1.0f, float h = 1.0f) noexcept = 0;
    virtual void SetScissors(const std::vector<AABB2>& scissors) noexcept = 0;

    virtual void ClearColor(const Rgba& color) noexcept = 0;
    virtual void ClearTargetColor(Texture* target, const Rgba& color) noexcept = 0;
    virtual void ClearTargetDepthStencilBuffer(Texture* target, bool depth = true, bool stencil = true, float depthValue = 1.0f, unsigned char stencilValue = 0) noexcept = 0;
    virtual void ClearDepthStencilBuffer() noexcept = 0;
    virtual void Present() noexcept = 0;

    virtual void DrawPoint(const Vertex3D& point) noexcept = 0;
    virtual void DrawPoint(const Vector3& point, const Rgba& color = Rgba::White, const Vector2& tex_coords = Vector2::Zero) noexcept = 0;
    virtual void DrawFrustum(const Frustum& frustum, const Rgba& color = Rgba::Yellow, const Vector2& tex_coords = Vector2::Zero) noexcept = 0;
    virtual void DrawWorldGridXZ(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::White, const Rgba& minor_color = Rgba::DarkGray) noexcept = 0;
    virtual void DrawWorldGridXY(float radius = 500.0f, float major_gridsize = 20.0f, float minor_gridsize = 5.0f, const Rgba& major_color = Rgba::White, const Rgba& minor_color = Rgba::DarkGray) noexcept = 0;
    virtual void DrawWorldGrid2D(const IntVector2& dimensions, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawWorldGrid2D(int width, int height, const Rgba& color = Rgba::White) noexcept = 0;

    virtual void DrawAxes(float maxlength = 1000.0f, bool disable_unit_depth = true) noexcept = 0;
    virtual void DrawDebugSphere(const Rgba& color) noexcept = 0;

    virtual void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo) noexcept = 0;
    virtual void Draw(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, std::size_t vertex_count) noexcept = 0;
    virtual void Draw(const PrimitiveType& topology, const std::vector<VertexCircle2D>& vbo) noexcept = 0;
    virtual void Draw(const PrimitiveType& topology, const std::vector<VertexCircle2D>& vbo, std::size_t vertex_count) noexcept = 0;
    virtual void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo) noexcept = 0;
    virtual void DrawIndexed(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<unsigned int>& ibo, std::size_t index_count, std::size_t startVertex = 0, std::size_t baseVertexLocation = 0) noexcept = 0;
    virtual void DrawInstanced(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<Vertex3DInstanced>& vbio, std::size_t instanceCount) noexcept = 0;
    virtual void DrawInstanced(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<Vertex3DInstanced>& vbio, std::size_t instanceCount, std::size_t vertexCount) noexcept = 0;
    virtual void DrawIndexedInstanced(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<Vertex3DInstanced>& vbio, const std::vector<unsigned int>& ibo, std::size_t instanceCount) noexcept = 0;
    virtual void DrawIndexedInstanced(const PrimitiveType& topology, const std::vector<Vertex3D>& vbo, const std::vector<Vertex3DInstanced>& vbio, const std::vector<unsigned int>& ibo, std::size_t instanceCount, std::size_t startIndexLocation, std::size_t baseVertexLocation, std::size_t startInstanceLocation) noexcept = 0;

    virtual void SetLightingEyePosition(const Vector3& position) noexcept = 0;
    virtual void SetAmbientLight(const Rgba& ambient) noexcept = 0;
    virtual void SetAmbientLight(const Rgba& color, float intensity) noexcept = 0;
    virtual void SetSpecGlossEmitFactors(Material* mat) noexcept = 0;
    virtual void SetUseVertexNormalsForLighting(bool value) noexcept = 0;

    [[nodiscard]] virtual const light_t& GetLight(unsigned int index) const noexcept = 0;
    virtual void SetPointLight(unsigned int index, const PointLightDesc& desc) noexcept = 0;
    virtual void SetDirectionalLight(unsigned int index, const DirectionalLightDesc& desc) noexcept = 0;
    virtual void SetSpotlight(unsigned int index, const SpotLightDesc& desc) noexcept = 0;

    [[nodiscard]] virtual RHIDeviceContext* GetDeviceContext() const noexcept = 0;
    [[nodiscard]] virtual RHIDevice* GetDevice() const noexcept = 0;
    [[nodiscard]] virtual RHIOutput* GetOutput() const noexcept = 0;
    [[nodiscard]] virtual RHIInstance* GetInstance() const noexcept = 0;

    [[nodiscard]] virtual ShaderProgram* GetShaderProgram(const std::string& nameOrFile) noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<ShaderProgram> CreateShaderProgramFromCsoFile(std::filesystem::path filepath, const PipelineStage& target) const noexcept = 0;
    [[nodiscard]] virtual std::unique_ptr<ShaderProgram> CreateShaderProgramFromDesc(ShaderProgramDesc&& desc) const noexcept = 0;
    virtual void CreateAndRegisterShaderProgramFromCsoFile(std::filesystem::path filepath, const PipelineStage& target) noexcept = 0;
    virtual void CreateAndRegisterRasterStateFromRasterDescription(const std::string& name, const RasterDesc& desc) noexcept = 0;
    virtual void SetRasterState(RasterState* raster) noexcept = 0;
    virtual void SetRasterState(FillMode fillmode, CullMode cullmode) noexcept = 0;
    [[nodiscard]] virtual RasterState* GetRasterState(const std::string& name) noexcept = 0;

    virtual void SetWireframeRaster(CullMode cullmode = CullMode::Back) noexcept = 0;
    virtual void SetSolidRaster(CullMode cullmode = CullMode::Back) noexcept = 0;

    virtual void CreateAndRegisterSamplerFromSamplerDescription(const std::string& name, const SamplerDesc& desc) noexcept = 0;
    [[nodiscard]] virtual Sampler* GetSampler(const std::string& name) noexcept = 0;
    virtual void SetSampler(Sampler* sampler) noexcept = 0;

    virtual void SetVSync(bool value) noexcept = 0;

    [[nodiscard]] virtual bool RegisterMaterial(std::filesystem::path filepath) noexcept = 0;
    virtual void RegisterMaterial(std::unique_ptr<Material> mat) noexcept = 0;
    virtual void RegisterMaterialsFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept = 0;
    virtual void ReloadMaterials() noexcept = 0;

    [[nodiscard]] virtual Material* GetMaterial(const std::string& nameOrFile) noexcept = 0;
    virtual void SetMaterial(Material* material) noexcept = 0;
    virtual void SetMaterial(const std::string& nameOrFile) noexcept = 0;
    virtual void ResetMaterial() noexcept = 0;

    [[nodiscard]] virtual bool IsTextureLoaded(const std::string& nameOrFile) const noexcept = 0;
    [[nodiscard]] virtual bool IsTextureNotLoaded(const std::string& nameOrFile) const noexcept = 0;

    [[nodiscard]] virtual bool RegisterShader(std::filesystem::path filepath) noexcept = 0;
    virtual void RegisterShader(std::unique_ptr<Shader> shader) noexcept = 0;

    [[nodiscard]] virtual Shader* GetShader(const std::string& nameOrFile) noexcept = 0;
    [[nodiscard]] virtual std::string GetShaderName(const std::filesystem::path filepath) noexcept = 0;

    virtual void SetComputeShader(Shader* shader) noexcept = 0;
    virtual void DispatchComputeJob(const ComputeJob& job) noexcept = 0;

    [[nodiscard]] virtual a2de::IFont* GetDefaultFont() noexcept = 0;
    [[nodiscard]] virtual a2de::IFont* GetFont(const std::string& nameOrFile) noexcept = 0;
    [[nodiscard]] virtual a2de::IFont* GetFontById(uint16_t index) noexcept = 0;
    [[nodiscard]] virtual std::size_t GetFontId([[maybe_unused]] const std::string& nameOrFile) noexcept = 0;
    virtual void RegisterFont(std::unique_ptr<a2de::IFont> font) noexcept = 0;
    
    [[nodiscard]] virtual bool RegisterFont(std::filesystem::path filepath) noexcept = 0;
    virtual void RegisterFontsFromFolder(std::filesystem::path folderpath, bool recursive = false) noexcept = 0;

    virtual void UpdateGameTime(TimeUtils::FPSeconds deltaSeconds) noexcept = 0;
    virtual void UpdateConstantBuffer(ConstantBuffer& buffer, void* const& data) noexcept = 0;

    virtual void ResetModelViewProjection() noexcept = 0;
    virtual void AppendModelMatrix(const Matrix4& modelMatrix) noexcept = 0;
    virtual void SetModelMatrix(const Matrix4& mat = Matrix4::I) noexcept = 0;
    virtual void SetViewMatrix(const Matrix4& mat = Matrix4::I) noexcept = 0;
    virtual void SetProjectionMatrix(const Matrix4& mat = Matrix4::I) noexcept = 0;
    virtual void SetOrthoProjection(const Vector2& leftBottom, const Vector2& rightTop, const Vector2& near_far) noexcept = 0;
    virtual void SetOrthoProjection(const Vector2& dimensions, const Vector2& origin, float nearz, float farz) noexcept = 0;
    virtual void SetOrthoProjectionFromViewHeight(float viewHeight, float aspectRatio, float nearz, float farz) noexcept = 0;
    virtual void SetOrthoProjectionFromViewWidth(float viewWidth, float aspectRatio, float nearz, float farz) noexcept = 0;
    virtual void SetOrthoProjectionFromCamera(const Camera3D& camera) noexcept = 0;
    virtual void SetPerspectiveProjection(const Vector2& vfovDegrees_aspect, const Vector2& nz_fz) noexcept = 0;
    virtual void SetPerspectiveProjectionFromCamera(const Camera3D& camera) noexcept = 0;
    virtual void SetCamera(const Camera3D& camera) noexcept = 0;
    virtual void SetCamera(const Camera2D& camera) noexcept = 0;
    [[nodiscard]] virtual Camera3D GetCamera() const noexcept = 0;

    [[nodiscard]] virtual Vector2 ConvertWorldToScreenCoords(const Vector3& worldCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector2 ConvertWorldToScreenCoords(const Vector2& worldCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector2 ConvertWorldToScreenCoords(const Camera3D& camera, const Vector3& worldCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector2 ConvertWorldToScreenCoords(const Camera2D& camera, const Vector2& worldCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector3 ConvertScreenToWorldCoords(const Vector2& mouseCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector3 ConvertScreenToWorldCoords(const Camera3D& camera, const Vector2& mouseCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector2 ConvertScreenToWorldCoords(const Camera2D& camera, const Vector2& mouseCoords) const noexcept = 0;

    [[nodiscard]] virtual Vector3 ConvertScreenToNdcCoords(const Camera3D& camera, const Vector2& mouseCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector2 ConvertScreenToNdcCoords(const Camera2D& camera, const Vector2& mouseCoords) const noexcept = 0;
    [[nodiscard]] virtual Vector3 ConvertScreenToNdcCoords(const Vector2& mouseCoords) const noexcept = 0;

    virtual void SetConstantBuffer(unsigned int index, ConstantBuffer* buffer) noexcept = 0;
    virtual void SetStructuredBuffer(unsigned int index, StructuredBuffer* buffer) noexcept = 0;
    virtual void SetComputeConstantBuffer(unsigned int index, ConstantBuffer* buffer) noexcept = 0;
    virtual void SetComputeStructuredBuffer(unsigned int index, StructuredBuffer* buffer) noexcept = 0;

    virtual void DrawBezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Rgba& color = Rgba::White, std::size_t resolution = 64u) noexcept = 0;
    virtual void DrawCube(const Vector3& position = Vector3::Zero, const Vector3& halfExtents = Vector3::One * 0.5f, const Rgba& color = Rgba::White) = 0;
    virtual void DrawQuad(const Vector3& position = Vector3::Zero, const Vector3& halfExtents = Vector3::XY_Axis * 0.5f, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_Axis, const Vector3& normalFront = Vector3::Z_Axis, const Vector3& worldUp = Vector3::Y_Axis) noexcept = 0;
    virtual void DrawQuad(const Rgba& frontColor, const Rgba& backColor, const Vector3& position = Vector3::Zero, const Vector3& halfExtents = Vector3::XY_Axis * 0.5f, const Vector4& texCoords = Vector4::ZW_Axis, const Vector3& normalFront = Vector3::Z_Axis, const Vector3& worldUp = Vector3::Y_Axis) noexcept = 0;
    virtual void DrawPoint2D(float pointX, float pointY, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawPoint2D(const Vector2& point, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawLine2D(float startX, float startY, float endX, float endY, const Rgba& color = Rgba::White, float thickness = 0.0f) noexcept = 0;
    virtual void DrawLine2D(const Vector2& start, const Vector2& end, const Rgba& color = Rgba::White, float thickness = 0.0f) noexcept = 0;
    virtual void DrawQuad2D(float left, float bottom, float right, float top, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_Axis) noexcept = 0;
    virtual void DrawQuad2D(const Vector2& position = Vector2::Zero, const Vector2& halfExtents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_Axis) noexcept = 0;
    virtual void DrawQuad2D(const Matrix4& transform, const Rgba& color = Rgba::White, const Vector4& texCoords = Vector4::ZW_Axis) noexcept = 0;
    virtual void DrawQuad2D(const Rgba& color) noexcept = 0;
    virtual void DrawQuad2D(const Vector4& texCoords) noexcept = 0;
    virtual void DrawQuad2D(const Rgba& color, const Vector4& texCoords) noexcept = 0;
    virtual void DrawCircle2D(float centerX, float centerY, float radius, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawCircle2D(const Matrix4& transform, float thickness, const Rgba& color = Rgba::White, float fade = 0.00025f) noexcept = 0;
    virtual void DrawCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawCircle2D(const Disc2& circle, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawFilledCircle2D(const Disc2& circle, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawFilledCircle2D(const Vector2& center, float radius, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawAABB2(const AABB2& bounds, const Rgba& edgeColor, const Rgba& fillColor, const Vector2& edgeHalfExtents = Vector2::Zero) noexcept = 0;
    virtual void DrawAABB2(const AABB2& bounds, const Rgba& edgeColor, const Rgba& fillColor, const Vector4& edgeHalfExtents) noexcept = 0;
    virtual void DrawAABB2(const Rgba& edgeColor, const Rgba& fillColor) noexcept = 0;
    virtual void DrawRoundedRectangle2D(const AABB2& bounds, const Rgba& color, float radius = 10.0f) noexcept = 0;
    virtual void DrawFilledRoundedRectangle2D(const AABB2& bounds, const Rgba& color, float radius) noexcept = 0;
    virtual void DrawFilledRoundedRectangle2D(const AABB2& bounds, const Rgba& color, float topLeftRadius, float topRightRadius, float bottomLeftRadius, float bottomRightRadius) noexcept = 0;
    virtual void DrawFilledRoundedRectangle2D(const AABB2& bounds, const Rgba& color, const Vector4& cornerRadii = Vector4(10.0f, 10.0f, 10.0f, 10.0f)) noexcept = 0;
    virtual void DrawFilledSquircle2D(const AABB2& bounds, const Rgba& color, float exponent = 10.0f) noexcept = 0;
    virtual void DrawOBB2(float orientationDegrees, const Rgba& edgeColor, const Rgba& fillColor = Rgba::NoAlpha) noexcept = 0;
    virtual void DrawOBB2(const OBB2& obb, const Rgba& edgeColor, const Rgba& fillColor = Rgba::NoAlpha, const Vector2& edgeHalfExtents = Vector2::Zero) noexcept = 0;
    virtual void DrawPolygon2D(float centerX, float centerY, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawPolygon2D(const Vector2& center, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawPolygon2D(const Polygon2& polygon, const Rgba& color = Rgba::White) = 0;
    virtual void DrawFilledPolygon2D(float centerX, float centerY, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawFilledPolygon2D(const Vector2& center, float radius, std::size_t numSides = 3, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawFilledPolygon2D(const Polygon2& polygon, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawX2D(const Vector2& position = Vector2::Zero, const Vector2& half_extents = Vector2(0.5f, 0.5f), const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawX2D(const Rgba& color) noexcept = 0;
    virtual void DrawArrow2D(const Vector2& position, const Rgba& color, const Vector2& direction, float tailLength, float arrowHeadSize = 0.1f) noexcept = 0;
    virtual void DrawTextLine(const a2de::IFont* font, const std::string& text, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawTextLine(const Matrix4& transform, const a2de::IFont* font, const std::string& text, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void DrawMultilineText(const a2de::IFont* font, const std::string& text, const Rgba& color = Rgba::White) noexcept = 0;
    virtual void AppendMultiLineTextBuffer(const a2de::IFont* font, const std::string& text, const Vector2& start_position, const Rgba& color, std::vector<Vertex3D>& vbo, std::vector<unsigned int>& ibo) noexcept = 0;

    virtual void CopyTexture(const Texture* src, Texture* dst) const noexcept = 0;
    virtual void ResizeBuffers() noexcept = 0;
    virtual void ClearState() noexcept = 0;

    virtual void RequestScreenShot() = 0;
    virtual void RequestScreenShot(std::filesystem::path saveLocation) = 0;

    [[nodiscard]] constexpr virtual unsigned int GetMatrixBufferIndex() const noexcept = 0;
    [[nodiscard]] constexpr virtual unsigned int GetTimeBufferIndex() const noexcept = 0;
    [[nodiscard]] constexpr virtual unsigned int GetLightingBufferIndex() const noexcept = 0;
    [[nodiscard]] constexpr virtual unsigned int GetConstantBufferStartIndex() const noexcept = 0;
    [[nodiscard]] constexpr virtual unsigned int GetStructuredBufferStartIndex() const noexcept = 0;
    [[nodiscard]] constexpr virtual unsigned int GetMaxLightCount() const noexcept = 0;

protected:
private:
    virtual void RegisterShaderProgram(const std::string& name, std::unique_ptr<class ShaderProgram> sp) noexcept = 0;
    virtual void RegisterShader(const std::string& name, std::unique_ptr<class Shader> shader) noexcept = 0;
    virtual void RegisterMaterial(const std::string& name, std::unique_ptr<class Material> mat) noexcept = 0;
    virtual void RegisterRasterState(const std::string& name, std::unique_ptr<class RasterState> raster) noexcept = 0;
    virtual void RegisterDepthStencilState(const std::string& name, std::unique_ptr<class DepthStencilState> depthstencil) noexcept = 0;
    virtual void RegisterSampler(const std::string& name, std::unique_ptr<class Sampler> sampler) noexcept = 0;
    virtual void RegisterFont(const std::string& name, std::unique_ptr<class a2de::IFont> font) noexcept = 0;

    friend class Shader;

};

class NullRendererService : public IRendererService {
public:
    virtual ~NullRendererService() noexcept {};

    void BeginRender([[maybe_unused]] Texture* color_target = nullptr, [[maybe_unused]] const Rgba& clear_color = Rgba::Black, [[maybe_unused]] Texture* depthstencil_target = nullptr) noexcept override {}
    void BeginRenderToBackbuffer([[maybe_unused]] const Rgba& clear_color = Rgba::Black) noexcept override {}
    void BeginHUDRender([[maybe_unused]] Camera2D& ui_camera, [[maybe_unused]] const Vector2& camera_position, [[maybe_unused]] float window_height) noexcept override {}

    [[nodiscard]] TimeUtils::FPSeconds GetGameFrameTime() const noexcept override { return TimeUtils::FPSeconds::zero(); }
    [[nodiscard]] TimeUtils::FPSeconds GetSystemFrameTime() const noexcept override { return TimeUtils::FPSeconds::zero(); }
    [[nodiscard]] TimeUtils::FPSeconds GetGameTime() const noexcept override { return TimeUtils::FPSeconds::zero(); }
    [[nodiscard]] TimeUtils::FPSeconds GetSystemTime() const noexcept override { return TimeUtils::FPSeconds::zero(); }

    void SetFullscreen([[maybe_unused]] bool isFullscreen) noexcept override {}
    void SetFullscreenMode() noexcept override {}
    void SetWindowedMode() noexcept override {}
    void SetWindowTitle([[maybe_unused]] const std::string& newTitle) const noexcept override {}
    [[nodiscard]] std::string GetWindowTitle() const noexcept override { return {}; }

    void SetWindowIcon([[maybe_unused]] void* iconResource) noexcept override {}

    [[nodiscard]] std::unique_ptr<VertexBuffer> CreateVertexBuffer([[maybe_unused]] const VertexBuffer::buffer_t& vbo) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<VertexCircleBuffer> CreateVertexCircleBuffer([[maybe_unused]] const VertexCircleBuffer::buffer_t& vbco) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<VertexBufferInstanced> CreateVertexBufferInstanced([[maybe_unused]] const VertexBufferInstanced::buffer_t& vbio) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<IndexBuffer> CreateIndexBuffer([[maybe_unused]] const IndexBuffer::buffer_t& ibo) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<ConstantBuffer> CreateConstantBuffer([[maybe_unused]] void* const& buffer, [[maybe_unused]] const std::size_t& buffer_size) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<StructuredBuffer> CreateStructuredBuffer([[maybe_unused]] const StructuredBuffer::buffer_t& sbo, [[maybe_unused]] std::size_t element_size, [[maybe_unused]] std::size_t element_count) const noexcept override { return {}; }

    [[nodiscard]] Texture* CreateOrGetTexture([[maybe_unused]] const std::filesystem::path& filepath, [[maybe_unused]] const IntVector3& dimensions) noexcept override { return nullptr; }
    void RegisterTexturesFromFolder([[maybe_unused]] std::filesystem::path folderpath, [[maybe_unused]] bool recursive = false) noexcept override {}
    [[nodiscard]] bool RegisterTexture([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<Texture> texture) noexcept override { return false; }
    void SetTexture([[maybe_unused]] Texture* texture, [[maybe_unused]] unsigned int registerIndex = 0) noexcept override {}

    [[nodiscard]] Texture* GetTexture([[maybe_unused]] const std::string& nameOrFile) noexcept override { return nullptr; }

    [[nodiscard]] std::unique_ptr<Texture> CreateDepthStencil([[maybe_unused]] const RHIDevice& owner, [[maybe_unused]] uint32_t width, [[maybe_unused]] uint32_t height) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> CreateDepthStencil([[maybe_unused]] const RHIDevice& owner, [[maybe_unused]] const IntVector2& dimensions) noexcept override { return {};}
    [[nodiscard]] std::unique_ptr<Texture> CreateRenderableDepthStencil([[maybe_unused]] const RHIDevice& owner, [[maybe_unused]] const IntVector2& dimensions) noexcept override { return {}; }

    [[nodiscard]] Texture* GetDefaultDepthStencil() const noexcept override { return nullptr; }
    void SetDepthStencilState([[maybe_unused]] DepthStencilState* depthstencil) noexcept override {}
    [[nodiscard]] DepthStencilState* GetDepthStencilState([[maybe_unused]] const std::string& name) noexcept override { return nullptr; }
    void CreateAndRegisterDepthStencilStateFromDepthStencilDescription([[maybe_unused]] const std::string& name, [[maybe_unused]] const DepthStencilDesc& desc) noexcept override {}
    void EnableDepth([[maybe_unused]] bool isDepthEnabled) noexcept override {}
    void EnableDepth() noexcept override {}
    void DisableDepth() noexcept override {}
    void EnableDepthWrite([[maybe_unused]] bool isDepthWriteEnabled) noexcept override {}
    void EnableDepthWrite() noexcept override {}
    void DisableDepthWrite() noexcept override {}

    void SetDepthComparison([[maybe_unused]] ComparisonFunction cf) noexcept override {}
    [[nodiscard]] ComparisonFunction GetDepthComparison() const noexcept override { return ComparisonFunction::Always; }
    void SetStencilFrontComparison([[maybe_unused]] ComparisonFunction cf) noexcept override {}
    void SetStencilBackComparison([[maybe_unused]] ComparisonFunction cf) noexcept override {}
    void EnableStencilWrite() noexcept override {}
    void DisableStencilWrite() noexcept override {}

    [[nodiscard]] Texture* Create1DTexture([[maybe_unused]] std::filesystem::path filepath, [[maybe_unused]] const BufferUsage& bufferUsage, [[maybe_unused]] const BufferBindUsage& bindUsage, [[maybe_unused]] const ImageFormat& imageFormat) noexcept override { return nullptr; }
    [[nodiscard]] std::unique_ptr<Texture> Create1DTextureFromMemory([[maybe_unused]] const unsigned char* data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> Create1DTextureFromMemory([[maybe_unused]] const std::vector<Rgba>& data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return {}; }
    [[nodiscard]] Texture* Create2DTexture([[maybe_unused]] std::filesystem::path filepath, [[maybe_unused]] const BufferUsage& bufferUsage, [[maybe_unused]] const BufferBindUsage& bindUsage, [[maybe_unused]] const ImageFormat& imageFormat) noexcept override { return nullptr; }
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromMemory([[maybe_unused]] const unsigned char* data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromMemory([[maybe_unused]] const std::vector<Rgba>& data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureFromMemory([[maybe_unused]] const void* data, [[maybe_unused]] std::size_t elementSize, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureArrayFromMemory([[maybe_unused]] const unsigned char* data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] unsigned int depth = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> Create2DTextureArrayFromFolder([[maybe_unused]] const std::filesystem::path folderpath) noexcept override { return {}; }
    [[nodiscard]] Texture* Create3DTexture([[maybe_unused]] std::filesystem::path filepath, [[maybe_unused]] const IntVector3& dimensions, [[maybe_unused]] const BufferUsage& bufferUsage, [[maybe_unused]] const BufferBindUsage& bindUsage, [[maybe_unused]] const ImageFormat& imageFormat) noexcept override { return nullptr; }
    [[nodiscard]] std::unique_ptr<Texture> Create3DTextureFromMemory([[maybe_unused]] const unsigned char* data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] unsigned int depth = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Texture> Create3DTextureFromMemory([[maybe_unused]] const std::vector<Rgba>& data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] unsigned int depth = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return {}; }
    [[nodiscard]] Texture* CreateTexture([[maybe_unused]] std::filesystem::path filepath, [[maybe_unused]] const IntVector3& dimensions, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return nullptr; }

    [[nodiscard]] std::unique_ptr<Texture> CreateVideoTextureFromMemory([[maybe_unused]] const unsigned char* data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::Ayuv, [[maybe_unused]] const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept override { return {}; };
    [[nodiscard]] std::unique_ptr<Texture> CreateVideoTextureFromMemory([[maybe_unused]] const std::vector<Rgba>& data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::Ayuv, [[maybe_unused]] const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept override { return {}; };
    [[nodiscard]] std::unique_ptr<Texture> CreateVideoTextureFromMemory([[maybe_unused]] const void* data, [[maybe_unused]] std::size_t elementSize, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::Ayuv, [[maybe_unused]] const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) const noexcept override { return {}; };
    [[nodiscard]] std::unique_ptr<Texture> CreateVideoTextureArrayFromMemory([[maybe_unused]] const unsigned char* data, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1, [[maybe_unused]] unsigned int depth = 1, [[maybe_unused]] const BufferUsage& bufferUsage = BufferUsage::Static, [[maybe_unused]] const BufferBindUsage& bindUsage = BufferBindUsage::Shader_Resource, [[maybe_unused]] const ImageFormat& imageFormat = ImageFormat::Ayuv, [[maybe_unused]] const ImageFormat& viewFormat = ImageFormat::R8G8B8A8_UNorm) noexcept override { return {}; };

    [[nodiscard]] std::shared_ptr<SpriteSheet> CreateSpriteSheet([[maybe_unused]] const std::filesystem::path& filepath, [[maybe_unused]] unsigned int width = 1, [[maybe_unused]] unsigned int height = 1) noexcept override { return {}; }
    [[nodiscard]] std::shared_ptr<SpriteSheet> CreateSpriteSheet([[maybe_unused]] const XMLElement& elem) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite([[maybe_unused]] std::filesystem::path filepath) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite([[maybe_unused]] const AnimatedSpriteDesc& desc) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite([[maybe_unused]] std::shared_ptr<SpriteSheet> sheet, [[maybe_unused]] const IntVector2& startSpriteCoords = IntVector2::Zero) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite([[maybe_unused]] std::shared_ptr<SpriteSheet> sheet, [[maybe_unused]] const XMLElement& elem) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<AnimatedSprite> CreateAnimatedSprite([[maybe_unused]] const XMLElement& elem) noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<Flipbook> CreateFlipbookFromFolder([[maybe_unused]] std::filesystem::path folderpath, [[maybe_unused]] TimeUtils::FPSeconds secondsPerFrame) noexcept override { return {}; };
    [[nodiscard]] std::unique_ptr<Flipbook> CreateFlipbookFromGif([[maybe_unused]] const FileUtils::Gif& gif) noexcept override { return {}; };

    void ClearRenderTargets([[maybe_unused]] const RenderTargetType& rtt) noexcept override {}
    void SetRenderTarget([[maybe_unused]] FrameBuffer& frameBuffer) noexcept override {}
    void SetRenderTarget([[maybe_unused]] Texture* color_target = nullptr, [[maybe_unused]] Texture* depthstencil_target = nullptr) noexcept override {}
    void SetRenderTargetsToBackBuffer() noexcept override {}
    [[nodiscard]] ViewportDesc GetCurrentViewport() const noexcept override { return ViewportDesc{}; }
    [[nodiscard]] float GetCurrentViewportAspectRatio() const noexcept override { return MathUtils::M_16_BY_9_RATIO; }
    [[nodiscard]] ViewportDesc GetViewport([[maybe_unused]] std::size_t index) const noexcept override { return ViewportDesc{}; }
    [[nodiscard]] unsigned int GetViewportCount() const noexcept override { return 0u; }
    [[nodiscard]] std::vector<ViewportDesc> GetAllViewports() const noexcept override { return {}; }

    void SetViewport([[maybe_unused]] const ViewportDesc& desc) noexcept override {}
    void SetViewport([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] float width, [[maybe_unused]] float height) noexcept override {}
    void SetViewport([[maybe_unused]] const AABB2& viewport) noexcept override {}
    void SetViewportAndScissor([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] float width, [[maybe_unused]] float height) noexcept override {}
    void SetViewportAndScissor([[maybe_unused]] const AABB2& viewport_and_scissor) noexcept override {}
    void SetViewports([[maybe_unused]] const std::vector<AABB3>& viewports) noexcept override {}
    void SetViewportAsPercent([[maybe_unused]] float x = 0.0f, [[maybe_unused]] float y = 0.0f, [[maybe_unused]] float w = 1.0f, [[maybe_unused]] float h = 1.0f) noexcept override {}
    void SetViewportAndScissorAsPercent([[maybe_unused]] float x = 0.0f, [[maybe_unused]] float y = 0.0f, [[maybe_unused]] float w = 1.0f, [[maybe_unused]] float h = 1.0f) noexcept override {}

    void EnableScissorTest() override {}
    void DisableScissorTest() override {}

    void SetScissor([[maybe_unused]] unsigned int x, [[maybe_unused]] unsigned int y, [[maybe_unused]] unsigned int width, [[maybe_unused]] unsigned int height) noexcept override {}
    void SetScissor([[maybe_unused]] const AABB2& scissor) noexcept override {}
    void SetScissorAsPercent([[maybe_unused]] float x = 0.0f, [[maybe_unused]] float y = 0.0f, [[maybe_unused]] float w = 1.0f, [[maybe_unused]] float h = 1.0f) noexcept override {}
    void SetScissorAndViewport([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] float width, [[maybe_unused]] float height) noexcept override {}
    void SetScissorAndViewport([[maybe_unused]] const AABB2& scissor_and_viewport) noexcept override {}
    void SetScissorAndViewportAsPercent([[maybe_unused]] float x = 0.0f, [[maybe_unused]] float y = 0.0f, [[maybe_unused]] float w = 1.0f, [[maybe_unused]] float h = 1.0f) noexcept override {}
    void SetScissors([[maybe_unused]] const std::vector<AABB2>& scissors) noexcept override {}

    void ClearColor([[maybe_unused]] const Rgba& color) noexcept override {}
    void ClearTargetColor([[maybe_unused]] Texture* target, [[maybe_unused]] const Rgba& color) noexcept override {}
    void ClearTargetDepthStencilBuffer([[maybe_unused]] Texture* target, [[maybe_unused]] bool depth = true, [[maybe_unused]] bool stencil = true, [[maybe_unused]] float depthValue = 1.0f, [[maybe_unused]] unsigned char stencilValue = 0) noexcept override {}
    void ClearDepthStencilBuffer() noexcept override {}
    void Present() noexcept override {}

    void DrawPoint([[maybe_unused]] const Vertex3D& point) noexcept override {}
    void DrawPoint([[maybe_unused]] const Vector3& point, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] const Vector2& tex_coords = Vector2::Zero) noexcept override {}
    void DrawFrustum([[maybe_unused]] const Frustum& frustum, [[maybe_unused]] const Rgba& color = Rgba::Yellow, [[maybe_unused]] const Vector2& tex_coords = Vector2::Zero) noexcept override {}
    void DrawWorldGridXZ([[maybe_unused]] float radius = 500.0f, [[maybe_unused]] float major_gridsize = 20.0f, [[maybe_unused]] float minor_gridsize = 5.0f, [[maybe_unused]] const Rgba& major_color = Rgba::White, [[maybe_unused]] const Rgba& minor_color = Rgba::DarkGray) noexcept override {}
    void DrawWorldGridXY([[maybe_unused]] float radius = 500.0f, [[maybe_unused]] float major_gridsize = 20.0f, [[maybe_unused]] float minor_gridsize = 5.0f, [[maybe_unused]] const Rgba& major_color = Rgba::White, [[maybe_unused]] const Rgba& minor_color = Rgba::DarkGray) noexcept override {}
    void DrawWorldGrid2D([[maybe_unused]] const IntVector2& dimensions, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawWorldGrid2D([[maybe_unused]] int width, [[maybe_unused]] int height, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}

    void DrawAxes([[maybe_unused]] float maxlength = 1000.0f, [[maybe_unused]] bool disable_unit_depth = true) noexcept override {}
    void DrawDebugSphere([[maybe_unused]] const Rgba& color) noexcept override {}

    void Draw([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo) noexcept override {}
    void Draw([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] std::size_t vertex_count) noexcept override {}
    void Draw([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<VertexCircle2D>& vbo) noexcept override {}
    void Draw([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<VertexCircle2D>& vbo, [[maybe_unused]] std::size_t vertex_count) noexcept override {}
    void DrawIndexed([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] const std::vector<unsigned int>& ibo) noexcept override {}
    void DrawIndexed([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] const std::vector<unsigned int>& ibo, [[maybe_unused]] std::size_t index_count, [[maybe_unused]] std::size_t startVertex = 0, [[maybe_unused]] std::size_t baseVertexLocation = 0) noexcept override {}
    void DrawInstanced([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] const std::vector<Vertex3DInstanced>& vbio, [[maybe_unused]] std::size_t instanceCount) noexcept override {};
    void DrawInstanced([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] const std::vector<Vertex3DInstanced>& vbio, [[maybe_unused]] std::size_t instanceCount, [[maybe_unused]] std::size_t vertexCount) noexcept override {};
    void DrawIndexedInstanced([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] const std::vector<Vertex3DInstanced>& vbio, [[maybe_unused]] const std::vector<unsigned int>& ibo, [[maybe_unused]] std::size_t instanceCount) noexcept override {}
    void DrawIndexedInstanced([[maybe_unused]] const PrimitiveType& topology, [[maybe_unused]] const std::vector<Vertex3D>& vbo, [[maybe_unused]] const std::vector<Vertex3DInstanced>& vbio, [[maybe_unused]] const std::vector<unsigned int>& ibo, [[maybe_unused]] std::size_t instanceCount, [[maybe_unused]] std::size_t startIndexLocation, [[maybe_unused]] std::size_t baseVertexLocation, [[maybe_unused]] std::size_t startInstanceLocation) noexcept override {};

    void SetLightingEyePosition([[maybe_unused]] const Vector3& position) noexcept override {}
    void SetAmbientLight([[maybe_unused]] const Rgba& ambient) noexcept override {}
    void SetAmbientLight([[maybe_unused]] const Rgba& color, [[maybe_unused]] float intensity) noexcept override {}
    void SetSpecGlossEmitFactors([[maybe_unused]] Material* mat) noexcept override {}
    void SetUseVertexNormalsForLighting([[maybe_unused]] bool value) noexcept override {}

    [[nodiscard]] const light_t& GetLight([[maybe_unused]] unsigned int index) const noexcept override {
        static light_t l{};
        return l;
    }
    void SetPointLight([[maybe_unused]] unsigned int index, [[maybe_unused]] const PointLightDesc& desc) noexcept override {}
    void SetDirectionalLight([[maybe_unused]] unsigned int index, [[maybe_unused]] const DirectionalLightDesc& desc) noexcept override {}
    void SetSpotlight([[maybe_unused]] unsigned int index, [[maybe_unused]] const SpotLightDesc& desc) noexcept override {}

    [[nodiscard]] RHIDeviceContext* GetDeviceContext() const noexcept override { return nullptr; }
    [[nodiscard]] RHIDevice* GetDevice() const noexcept override { return nullptr; }
    [[nodiscard]] RHIOutput* GetOutput() const noexcept override { return nullptr; }
    [[nodiscard]] RHIInstance* GetInstance() const noexcept override { return nullptr; }

    [[nodiscard]] ShaderProgram* GetShaderProgram([[maybe_unused]] const std::string& nameOrFile) noexcept override { return nullptr; }
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromCsoFile([[maybe_unused]] std::filesystem::path filepath, [[maybe_unused]] const PipelineStage& target) const noexcept override { return {}; }
    [[nodiscard]] std::unique_ptr<ShaderProgram> CreateShaderProgramFromDesc([[maybe_unused]] ShaderProgramDesc&& desc) const noexcept override { return {}; }
    void CreateAndRegisterShaderProgramFromCsoFile([[maybe_unused]] std::filesystem::path filepath, [[maybe_unused]] const PipelineStage& target) noexcept override {}
    void CreateAndRegisterRasterStateFromRasterDescription([[maybe_unused]] const std::string& name, [[maybe_unused]] const RasterDesc& desc) noexcept override {}
    void SetRasterState([[maybe_unused]] RasterState* raster) noexcept override {}
    void SetRasterState([[maybe_unused]] FillMode fillmode, [[maybe_unused]] CullMode cullmode) noexcept override {}
    [[nodiscard]] RasterState* GetRasterState([[maybe_unused]] const std::string& name) noexcept override { return nullptr; }

    void SetWireframeRaster([[maybe_unused]] CullMode cullmode = CullMode::Back) noexcept override {}
    void SetSolidRaster([[maybe_unused]] CullMode cullmode = CullMode::Back) noexcept override {}

    void CreateAndRegisterSamplerFromSamplerDescription([[maybe_unused]] const std::string& name, [[maybe_unused]] const SamplerDesc& desc) noexcept override {}
    [[nodiscard]] Sampler* GetSampler([[maybe_unused]] const std::string& name) noexcept override { return nullptr; }
    void SetSampler([[maybe_unused]] Sampler* sampler) noexcept override {}

    void SetVSync([[maybe_unused]] bool value) noexcept override {}

    [[nodiscard]] bool RegisterMaterial([[maybe_unused]] std::filesystem::path filepath) noexcept override { return false; }
    void RegisterMaterial([[maybe_unused]] std::unique_ptr<Material> mat) noexcept override {}
    void RegisterMaterialsFromFolder([[maybe_unused]] std::filesystem::path folderpath, [[maybe_unused]] bool recursive = false) noexcept override {}
    void ReloadMaterials() noexcept override {}

    [[nodiscard]] Material* GetMaterial([[maybe_unused]] const std::string& nameOrFile) noexcept override { return nullptr; }
    void SetMaterial([[maybe_unused]] Material* material) noexcept override {}
    void SetMaterial([[maybe_unused]] const std::string& nameOrFile) noexcept override {}
    void ResetMaterial() noexcept override {}

    [[nodiscard]] bool IsTextureLoaded([[maybe_unused]] const std::string& nameOrFile) const noexcept override { return false; }
    [[nodiscard]] bool IsTextureNotLoaded([[maybe_unused]] const std::string& nameOrFile) const noexcept override { return false; }

    [[nodiscard]] bool RegisterShader([[maybe_unused]] std::filesystem::path filepath) noexcept override { return false; }
    void RegisterShader([[maybe_unused]] std::unique_ptr<Shader> shader) noexcept override {}

    [[nodiscard]] Shader* GetShader([[maybe_unused]] const std::string& nameOrFile) noexcept override { return nullptr; }
    [[nodiscard]] std::string GetShaderName([[maybe_unused]] const std::filesystem::path filepath) noexcept override { return {}; }

    void SetComputeShader([[maybe_unused]] Shader* shader) noexcept override {}
    void DispatchComputeJob([[maybe_unused]] const ComputeJob& job) noexcept override {}

    [[nodiscard]] a2de::IFont* GetDefaultFont() noexcept override { return nullptr; }
    [[nodiscard]] a2de::IFont* GetFont([[maybe_unused]] const std::string& nameOrFile) noexcept override { return nullptr; }
    [[nodiscard]] a2de::IFont* GetFontById([[maybe_unused]] uint16_t index) noexcept override { return nullptr; };
    [[nodiscard]] std::size_t GetFontId([[maybe_unused]] const std::string& nameOrFile) noexcept override { return 0; }
    void RegisterFont([[maybe_unused]] std::unique_ptr<a2de::IFont> font) noexcept override {}
    [[nodiscard]] bool RegisterFont([[maybe_unused]] std::filesystem::path filepath) noexcept override { return false; }
    void RegisterFontsFromFolder([[maybe_unused]] std::filesystem::path folderpath, [[maybe_unused]] bool recursive = false) noexcept override {}

    void UpdateGameTime([[maybe_unused]] TimeUtils::FPSeconds deltaSeconds) noexcept override {}
    void UpdateConstantBuffer([[maybe_unused]] ConstantBuffer& buffer, [[maybe_unused]] void* const& data) noexcept override {}
    void ResetModelViewProjection() noexcept override {}
    void AppendModelMatrix([[maybe_unused]] const Matrix4& modelMatrix) noexcept override {}
    void SetModelMatrix([[maybe_unused]] const Matrix4& mat = Matrix4::I) noexcept override {}
    void SetViewMatrix([[maybe_unused]] const Matrix4& mat = Matrix4::I) noexcept override {}
    void SetProjectionMatrix([[maybe_unused]] const Matrix4& mat = Matrix4::I) noexcept override {}
    void SetOrthoProjection([[maybe_unused]] const Vector2& leftBottom, [[maybe_unused]] const Vector2& rightTop, [[maybe_unused]] const Vector2& near_far) noexcept override {}
    void SetOrthoProjection([[maybe_unused]] const Vector2& dimensions, [[maybe_unused]] const Vector2& origin, [[maybe_unused]] float nearz, [[maybe_unused]] float farz) noexcept override {}
    void SetOrthoProjectionFromViewHeight([[maybe_unused]] float viewHeight, [[maybe_unused]] float aspectRatio, [[maybe_unused]] float nearz, [[maybe_unused]] float farz) noexcept override {}
    void SetOrthoProjectionFromViewWidth([[maybe_unused]] float viewWidth, [[maybe_unused]] float aspectRatio, [[maybe_unused]] float nearz, [[maybe_unused]] float farz) noexcept override {}
    void SetOrthoProjectionFromCamera([[maybe_unused]] const Camera3D& camera) noexcept override {}
    void SetPerspectiveProjection([[maybe_unused]] const Vector2& vfovDegrees_aspect, [[maybe_unused]] const Vector2& nz_fz) noexcept override {}
    void SetPerspectiveProjectionFromCamera([[maybe_unused]] const Camera3D& camera) noexcept override {}
    void SetCamera([[maybe_unused]] const Camera3D& camera) noexcept override {}
    void SetCamera([[maybe_unused]] const Camera2D& camera) noexcept override {}
    [[nodiscard]] Camera3D GetCamera() const noexcept override { return {}; }

    [[nodiscard]] Vector2 ConvertWorldToScreenCoords([[maybe_unused]] const Vector3& worldCoords) const noexcept override { return Vector2::Zero; }
    [[nodiscard]] Vector2 ConvertWorldToScreenCoords([[maybe_unused]] const Vector2& worldCoords) const noexcept override { return Vector2::Zero; }
    [[nodiscard]] Vector2 ConvertWorldToScreenCoords([[maybe_unused]] const Camera3D& camera, [[maybe_unused]] const Vector3& worldCoords) const noexcept override { return Vector2::Zero; }
    [[nodiscard]] Vector2 ConvertWorldToScreenCoords([[maybe_unused]] const Camera2D& camera, [[maybe_unused]] const Vector2& worldCoords) const noexcept override { return Vector2::Zero; }
    [[nodiscard]] Vector3 ConvertScreenToWorldCoords([[maybe_unused]] const Vector2& mouseCoords) const noexcept override { return Vector3::Zero; }
    [[nodiscard]] Vector3 ConvertScreenToWorldCoords([[maybe_unused]] const Camera3D& camera, [[maybe_unused]] const Vector2& mouseCoords) const noexcept override { return Vector3::Zero; }
    [[nodiscard]] Vector2 ConvertScreenToWorldCoords([[maybe_unused]] const Camera2D& camera, [[maybe_unused]] const Vector2& mouseCoords) const noexcept override { return Vector2::Zero; }

    [[nodiscard]] Vector3 ConvertScreenToNdcCoords([[maybe_unused]] const Camera3D& camera, [[maybe_unused]] const Vector2& mouseCoords) const noexcept override { return Vector3::Zero; }
    [[nodiscard]] Vector2 ConvertScreenToNdcCoords([[maybe_unused]] const Camera2D& camera, [[maybe_unused]] const Vector2& mouseCoords) const noexcept override { return Vector2::Zero; }
    [[nodiscard]] Vector3 ConvertScreenToNdcCoords([[maybe_unused]] const Vector2& mouseCoords) const noexcept override { return Vector3::Zero; }

    void SetConstantBuffer([[maybe_unused]] unsigned int index, [[maybe_unused]] ConstantBuffer* buffer) noexcept override {}
    void SetStructuredBuffer([[maybe_unused]] unsigned int index, [[maybe_unused]] StructuredBuffer* buffer) noexcept override {}
    void SetComputeConstantBuffer([[maybe_unused]] unsigned int index, [[maybe_unused]] ConstantBuffer* buffer) noexcept override {}
    void SetComputeStructuredBuffer([[maybe_unused]] unsigned int index, [[maybe_unused]] StructuredBuffer* buffer) noexcept override {}

    void DrawBezier([[maybe_unused]] const Vector2& p0, [[maybe_unused]] const Vector2& p1, [[maybe_unused]] const Vector2& p2, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] std::size_t resolution = 64u) noexcept override {}
    void DrawCube([[maybe_unused]] const Vector3& position = Vector3::Zero, [[maybe_unused]] const Vector3& halfExtents = Vector3::One * 0.5f, [[maybe_unused]] const Rgba& color = Rgba::White) override {}
    void DrawQuad([[maybe_unused]] const Vector3& position = Vector3::Zero, [[maybe_unused]] const Vector3& halfExtents = Vector3::XY_Axis * 0.5f, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] const Vector4& texCoords = Vector4::ZW_Axis, [[maybe_unused]] const Vector3& normalFront = Vector3::Z_Axis, [[maybe_unused]] const Vector3& worldUp = Vector3::Y_Axis) noexcept override {}
    void DrawQuad([[maybe_unused]] const Rgba& frontColor, [[maybe_unused]] const Rgba& backColor, [[maybe_unused]] const Vector3& position = Vector3::Zero, [[maybe_unused]] const Vector3& halfExtents = Vector3::XY_Axis * 0.5f, [[maybe_unused]] const Vector4& texCoords = Vector4::ZW_Axis, [[maybe_unused]] const Vector3& normalFront = Vector3::Z_Axis, [[maybe_unused]] const Vector3& worldUp = Vector3::Y_Axis) noexcept override {}
    void DrawPoint2D([[maybe_unused]] float pointX, [[maybe_unused]] float pointY, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawPoint2D([[maybe_unused]] const Vector2& point, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawLine2D([[maybe_unused]] float startX, [[maybe_unused]] float startY, [[maybe_unused]] float endX, [[maybe_unused]] float endY, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] float thickness = 0.0f) noexcept override {}
    void DrawLine2D([[maybe_unused]] const Vector2& start, [[maybe_unused]] const Vector2& end, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] float thickness = 0.0f) noexcept override {}
    void DrawQuad2D([[maybe_unused]] float left, [[maybe_unused]] float bottom, [[maybe_unused]] float right, [[maybe_unused]] float top, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] const Vector4& texCoords = Vector4::ZW_Axis) noexcept override {}
    void DrawQuad2D([[maybe_unused]] const Vector2& position = Vector2::Zero, [[maybe_unused]] const Vector2& halfExtents = Vector2(0.5f, 0.5f), [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] const Vector4& texCoords = Vector4::ZW_Axis) noexcept override {}
    void DrawQuad2D([[maybe_unused]] const Matrix4& transform, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] const Vector4& texCoords = Vector4::ZW_Axis) noexcept override {}
    void DrawQuad2D([[maybe_unused]] const Rgba& color) noexcept override {}
    void DrawQuad2D([[maybe_unused]] const Vector4& texCoords) noexcept override {}
    void DrawQuad2D([[maybe_unused]] const Rgba& color, [[maybe_unused]] const Vector4& texCoords) noexcept override {}
    void DrawCircle2D([[maybe_unused]] float centerX, [[maybe_unused]] float centerY, [[maybe_unused]] float radius, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawCircle2D([[maybe_unused]] const Matrix4& transform, [[maybe_unused]] float thickness, [[maybe_unused]] const Rgba& color = Rgba::White, [[maybe_unused]] float fade = 0.00025f) noexcept override {}
    void DrawCircle2D([[maybe_unused]] const Vector2& center, [[maybe_unused]] float radius, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawCircle2D([[maybe_unused]] const Disc2& circle, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawFilledCircle2D([[maybe_unused]] const Disc2& circle, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawFilledCircle2D([[maybe_unused]] const Vector2& center, [[maybe_unused]] float radius, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawAABB2([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& edgeColor, [[maybe_unused]] const Rgba& fillColor, [[maybe_unused]] const Vector2& edgeHalfExtents = Vector2::Zero) noexcept override {}
    void DrawAABB2([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& edgeColor, [[maybe_unused]] const Rgba& fillColor, [[maybe_unused]] const Vector4& edgeHalfExtents) noexcept override {}
    void DrawAABB2([[maybe_unused]] const Rgba& edgeColor, [[maybe_unused]] const Rgba& fillColor) noexcept override {}
    void DrawRoundedRectangle2D([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& color, [[maybe_unused]]float radius) noexcept override {}
    void DrawFilledRoundedRectangle2D([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& color, [[maybe_unused]] float radius) noexcept override {}
    void DrawFilledRoundedRectangle2D([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& color, [[maybe_unused]] float topLeftRadiusf, [[maybe_unused]] float topRightRadius, [[maybe_unused]] float bottomLeftRadius, [[maybe_unused]] float bottomRightRadius) noexcept override {}
    void DrawFilledRoundedRectangle2D([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& color, [[maybe_unused]] const Vector4& cornerRadii = Vector4(10.0f, 10.0f, 10.0f, 10.0f)) noexcept override {}
    void DrawFilledSquircle2D([[maybe_unused]] const AABB2& bounds, [[maybe_unused]] const Rgba& color, [[maybe_unused]] float exponent = 10.0f) noexcept override {};
    void DrawOBB2([[maybe_unused]] float orientationDegrees, [[maybe_unused]] const Rgba& edgeColor, [[maybe_unused]] const Rgba& fillColor = Rgba::NoAlpha) noexcept override {}
    void DrawOBB2([[maybe_unused]] const OBB2& obb, [[maybe_unused]] const Rgba& edgeColor, [[maybe_unused]] const Rgba& fillColor = Rgba::NoAlpha, [[maybe_unused]] const Vector2& edgeHalfExtents = Vector2::Zero) noexcept override {}
    void DrawPolygon2D([[maybe_unused]] float centerX, [[maybe_unused]] float centerY, [[maybe_unused]] float radius, [[maybe_unused]] std::size_t numSides = 3, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawPolygon2D([[maybe_unused]] const Vector2& center, [[maybe_unused]] float radius, [[maybe_unused]] std::size_t numSides = 3, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawPolygon2D([[maybe_unused]] const Polygon2& polygon, [[maybe_unused]] const Rgba& color = Rgba::White) override {}
    void DrawFilledPolygon2D([[maybe_unused]] float centerX, [[maybe_unused]] float centerY, [[maybe_unused]] float radius, [[maybe_unused]] std::size_t numSides = 3, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawFilledPolygon2D([[maybe_unused]] const Vector2& center, [[maybe_unused]] float radius, [[maybe_unused]] std::size_t numSides = 3, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawFilledPolygon2D([[maybe_unused]] const Polygon2& polygon, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawX2D([[maybe_unused]] const Vector2& position = Vector2::Zero, [[maybe_unused]] const Vector2& half_extents = Vector2(0.5f, 0.5f), [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawX2D([[maybe_unused]] const Rgba& color) noexcept override {}
    void DrawArrow2D([[maybe_unused]] const Vector2& position, [[maybe_unused]] const Rgba& color, [[maybe_unused]] const Vector2& direction, [[maybe_unused]] float tailLength, [[maybe_unused]] float arrowHeadSize = 0.1f) noexcept override {}

    void DrawTextLine([[maybe_unused]] const a2de::IFont* font, [[maybe_unused]] const std::string& text, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawTextLine([[maybe_unused]] const Matrix4& transform, [[maybe_unused]] const a2de::IFont* font, [[maybe_unused]] const std::string& text, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void DrawMultilineText([[maybe_unused]] const a2de::IFont* font, [[maybe_unused]] const std::string& text, [[maybe_unused]] const Rgba& color = Rgba::White) noexcept override {}
    void AppendMultiLineTextBuffer([[maybe_unused]] const a2de::IFont* font, [[maybe_unused]] const std::string& text, [[maybe_unused]] const Vector2& start_position, [[maybe_unused]] const Rgba& color, [[maybe_unused]] std::vector<Vertex3D>& vbo, [[maybe_unused]] std::vector<unsigned int>& ibo) noexcept override {}

    void CopyTexture([[maybe_unused]] const Texture* src, [[maybe_unused]] Texture* dst) const noexcept override {}
    void ResizeBuffers() noexcept override {}
    void ClearState() noexcept override {}

    void RequestScreenShot() override {}
    void RequestScreenShot([[maybe_unused]] std::filesystem::path saveLocation) override {}

    [[nodiscard]] [[maybe_unused]] constexpr unsigned int GetMatrixBufferIndex() const noexcept override { return 0; };
    [[nodiscard]] [[maybe_unused]] constexpr unsigned int GetTimeBufferIndex() const noexcept override { return 1; };
    [[nodiscard]] [[maybe_unused]] constexpr unsigned int GetLightingBufferIndex() const noexcept override { return 2; };
    [[nodiscard]] [[maybe_unused]] constexpr unsigned int GetConstantBufferStartIndex() const noexcept override { return 3; };
    [[nodiscard]] [[maybe_unused]] constexpr unsigned int GetStructuredBufferStartIndex() const noexcept override { return 64; };
    [[nodiscard]] [[maybe_unused]] constexpr unsigned int GetMaxLightCount() const noexcept override { return 16; };

protected:
private:
    void RegisterShaderProgram([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class ShaderProgram> sp) noexcept override {}
    void RegisterShader([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class Shader> shader) noexcept override {}
    void RegisterMaterial([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class Material> mat) noexcept override {}
    void RegisterRasterState([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class RasterState> raster) noexcept override {}
    void RegisterDepthStencilState([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class DepthStencilState> depthstencil) noexcept override {}
    void RegisterSampler([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class Sampler> sampler) noexcept override {}
    void RegisterFont([[maybe_unused]] const std::string& name, [[maybe_unused]] std::unique_ptr<class a2de::IFont> font) noexcept override {}

};
