#pragma once

#include <exception>
#include <memory>
#include <vector>

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <d3dcompiler.h>

#pragma warning(disable:4005) //dwrite.h conflicts with winerror.h
#pragma warning(disable:4458)

#include "FW1FontWrapper\FW1FontWrapper.h"
#include "shader.h"
#include <string>

using namespace DirectX;
using namespace DirectX::PackedVector;

using vec4_t = XMFLOAT4;
using vec3_t = XMFLOAT3;
using vec2_t = XMFLOAT2;

using color = XMVECTORF32;

template <typename Ty>
inline void safe_release(Ty& comPtr);

struct vertex;

typedef HRESULT(__stdcall* d3d_compile_t)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3D10Blob** ppCode, ID3D10Blob* ppErrorMsgs);

struct batch
{
    batch(std::size_t count, D3D11_PRIMITIVE_TOPOLOGY topology)
		: count(count), topology(topology)
	{}

	std::size_t count;
	D3D11_PRIMITIVE_TOPOLOGY topology;
};

class render_list
{	
	friend class renderer;
public:
	render_list() = delete;

	render_list(IFW1Factory* font_factory, std::size_t max_vertices = 0) {
		vertices.reserve(max_vertices);
		throw_if_failed(font_factory->CreateTextGeometry(&text_geometry));
	}

	~render_list() {
		safe_release(text_geometry);
	}

	void clear() {
		vertices.clear();
		batches.clear();
        text_geometry->Clear();
	}

protected:
	std::vector<vertex>	vertices;
	std::vector<batch>	batches;

	IFW1TextGeometry* text_geometry;
};

class renderer : public std::enable_shared_from_this<renderer>
{
public:
	renderer(IDXGISwapChain* swapchain, ID3D11Device* direct_3d_device, const std::wstring& default_font_family = L"Verdana");

	~renderer();

	void begin();

	void end();

	void draw(const render_list* render_list);

	void draw();

	void add_vertex(const render_list* render_list, vertex& vertex, D3D11_PRIMITIVE_TOPOLOGY topology);

	void add_vertex(vertex& vertex, D3D11_PRIMITIVE_TOPOLOGY topology);

	template <std::size_t N>
	void add_vertices(const render_list* render_list, vertex(&vertexArr)[N], D3D11_PRIMITIVE_TOPOLOGY topology);

	template <std::size_t N>
	void add_vertices(vertex(&vertexArr)[N], D3D11_PRIMITIVE_TOPOLOGY topology);

	void draw_text(const render_list* render_list, const vec2_t& pos, const std::wstring& text, const color& color, std::uint32_t flags = FW1_LEFT, float fontSize = 10.f, const std::wstring& fontFamily = {});

	void draw_text(const vec2_t& pos, const std::wstring& text, const color& color, std::uint32_t flags = FW1_LEFT, float fontSize = 10.f, const std::wstring& fontFamily = {});

	vec2_t get_text_extent(const std::wstring& text, float fontSize = 10.f, const std::wstring& fontFamily = {}) const;

	void draw_pixel(const vec2_t& pos, const color& color);

	void draw_pixel(const render_list* render_list, const vec2_t& pos, const color& color);

	void draw_line(const vec2_t& from, const vec2_t& to, const color& color);

	void draw_line(const render_list* render_list, const vec2_t& from, const vec2_t& to, const color& color);

	void draw_filled_rect(const vec4_t& rect, const color& color);

	void draw_filled_rect(const render_list* render_list, const vec4_t& rect, const color& color);

	void draw_rect(const vec4_t& rect, float strokeWidth, const color& color);

	void draw_rect(const render_list* render_list, const vec4_t& rect, float strokeWidth, const color& color);

	void draw_outlined_rect(const vec4_t& rect, float strokeWidth, const color& strokecolor, const color& fillcolor);

	void draw_outlined_rect(const render_list* render_list, const vec4_t& rect, float strokeWidth, const color& strokecolor, const color& fillcolor);

	void draw_circle(const vec2_t& pos, float radius, const color& color);

	void draw_circle(const render_list* render_list, const vec2_t& pos, float radius, const color& color);

	IFW1Factory* get_font_factory() const;

	std::shared_ptr<renderer> ptr();

    ID3D11DeviceContext* immediate_context;

    float width{};

    float height{};
private:
	ID3D11Device* direct_3d_device;

	ID3D11InputLayout* input_layout;

	ID3D11BlendState* blend_state;

	ID3D11VertexShader* vertex_shader;

	ID3D11PixelShader* pixel_shader;

	ID3D11Buffer* vertex_buffer;

	ID3D11Buffer* screen_projection_buffer;

	IFW1Factory* font_factory;

	IFW1FontWrapper* font_wrapper;

	XMMATRIX projection;

	render_list* render_list_ptr;

	std::size_t max_vertices;

	std::wstring default_font_family;
};

template <std::size_t N>
void renderer::add_vertices(const render_list* render_list, vertex(&vertexArr)[N], D3D11_PRIMITIVE_TOPOLOGY topology) {
	if (std::size(render_list->vertices) + N >= max_vertices)
		(&this->render_list_ptr == &render_list) ? draw(render_list) : throw std::exception(
			"Renderer::add_vertex - vertex buffer exhausted! Increase the size of the vertex buffer or add a custom implementation.");

	if (std::empty(render_list->batches) || render_list->batches.back().topology != topology)
		render_list->batches.emplace_back(0, topology);

	render_list->batches.back().count += N;

	render_list->vertices.resize(std::size(render_list->vertices) + N);
	std::memcpy(&render_list->vertices[std::size(render_list->vertices) - N], &vertexArr[0], N * sizeof(vertex));

	switch (topology)
	{
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
	{
		vertex seperator{};
		add_vertex(seperator, D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);
		break;
	}
	default:
		break;
	}
}

template <std::size_t N>
void renderer::add_vertices(vertex(&vertexArr)[N], D3D11_PRIMITIVE_TOPOLOGY topology) {
	add_vertices(render_list, vertexArr, topology);
}

template <typename Ty>
inline void safe_release(Ty& comPtr)
{
	static_assert(std::is_pointer<Ty>::value,
		"safe_release - comPtr not a pointer.");

	static_assert(std::is_base_of<IUnknown, std::remove_pointer<Ty>::type>::value,
		"safe_release - remove_ptr<comPtr>::type is not a com object.");

	if (comPtr) {
		comPtr->Release();
		comPtr = nullptr;
	}
}

struct vertex {
    vertex() = default;
    vertex(float x, float y, float z, color col)
		: pos(x, y, z), col(col)
	{}

	vec3_t pos;
	color col;
};

namespace colors {
    // Standard colors (Red/Green/Blue/Alpha)
    XMGLOBALCONST XMVECTORF32 AliceBlue = { { { 0.941176534f, 0.972549081f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 AntiqueWhite = { { { 0.980392218f, 0.921568692f, 0.843137324f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Aqua = { { { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Aquamarine = { { { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Azure = { { { 0.941176534f, 1.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Beige = { { { 0.960784376f, 0.960784376f, 0.862745166f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Bisque = { { { 1.000000000f, 0.894117713f, 0.768627524f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Black = { { { 0.000000000f, 0.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 BlanchedAlmond = { { { 1.000000000f, 0.921568692f, 0.803921640f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Blue = { { { 0.000000000f, 0.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 BlueViolet = { { { 0.541176498f, 0.168627456f, 0.886274576f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Brown = { { { 0.647058845f, 0.164705887f, 0.164705887f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 BurlyWood = { { { 0.870588303f, 0.721568644f, 0.529411793f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 CadetBlue = { { { 0.372549027f, 0.619607866f, 0.627451003f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Chartreuse = { { { 0.498039246f, 1.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Chocolate = { { { 0.823529482f, 0.411764741f, 0.117647067f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Coral = { { { 1.000000000f, 0.498039246f, 0.313725501f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 CornflowerBlue = { { { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Cornsilk = { { { 1.000000000f, 0.972549081f, 0.862745166f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Crimson = { { { 0.862745166f, 0.078431375f, 0.235294133f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Cyan = { { { 0.000000000f, 1.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkBlue = { { { 0.000000000f, 0.000000000f, 0.545098066f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkCyan = { { { 0.000000000f, 0.545098066f, 0.545098066f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkGoldenrod = { { { 0.721568644f, 0.525490224f, 0.043137256f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkGray = { { { 0.662745118f, 0.662745118f, 0.662745118f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkGreen = { { { 0.000000000f, 0.392156899f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkKhaki = { { { 0.741176486f, 0.717647076f, 0.419607878f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkMagenta = { { { 0.545098066f, 0.000000000f, 0.545098066f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkOliveGreen = { { { 0.333333343f, 0.419607878f, 0.184313729f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkOrange = { { { 1.000000000f, 0.549019635f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkOrchid = { { { 0.600000024f, 0.196078449f, 0.800000072f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkRed = { { { 0.545098066f, 0.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkSalmon = { { { 0.913725555f, 0.588235319f, 0.478431404f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkSeaGreen = { { { 0.560784340f, 0.737254918f, 0.545098066f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkSlateBlue = { { { 0.282352954f, 0.239215702f, 0.545098066f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkSlateGray = { { { 0.184313729f, 0.309803933f, 0.309803933f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkTurquoise = { { { 0.000000000f, 0.807843208f, 0.819607913f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DarkViolet = { { { 0.580392182f, 0.000000000f, 0.827451050f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DeepPink = { { { 1.000000000f, 0.078431375f, 0.576470613f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DeepSkyBlue = { { { 0.000000000f, 0.749019623f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DimGray = { { { 0.411764741f, 0.411764741f, 0.411764741f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 DodgerBlue = { { { 0.117647067f, 0.564705908f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Firebrick = { { { 0.698039234f, 0.133333340f, 0.133333340f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 FloralWhite = { { { 1.000000000f, 0.980392218f, 0.941176534f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 ForestGreen = { { { 0.133333340f, 0.545098066f, 0.133333340f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Fuchsia = { { { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Gainsboro = { { { 0.862745166f, 0.862745166f, 0.862745166f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 GhostWhite = { { { 0.972549081f, 0.972549081f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Gold = { { { 1.000000000f, 0.843137324f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Goldenrod = { { { 0.854902029f, 0.647058845f, 0.125490203f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Gray = { { { 0.501960814f, 0.501960814f, 0.501960814f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Green = { { { 0.000000000f, 0.501960814f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 GreenYellow = { { { 0.678431392f, 1.000000000f, 0.184313729f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Honeydew = { { { 0.941176534f, 1.000000000f, 0.941176534f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 HotPink = { { { 1.000000000f, 0.411764741f, 0.705882370f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 IndianRed = { { { 0.803921640f, 0.360784322f, 0.360784322f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Indigo = { { { 0.294117659f, 0.000000000f, 0.509803951f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Ivory = { { { 1.000000000f, 1.000000000f, 0.941176534f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Khaki = { { { 0.941176534f, 0.901960850f, 0.549019635f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Lavender = { { { 0.901960850f, 0.901960850f, 0.980392218f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LavenderBlush = { { { 1.000000000f, 0.941176534f, 0.960784376f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LawnGreen = { { { 0.486274540f, 0.988235354f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LemonChiffon = { { { 1.000000000f, 0.980392218f, 0.803921640f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightBlue = { { { 0.678431392f, 0.847058892f, 0.901960850f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightCoral = { { { 0.941176534f, 0.501960814f, 0.501960814f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightCyan = { { { 0.878431439f, 1.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightGoldenrodYellow = { { { 0.980392218f, 0.980392218f, 0.823529482f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightGreen = { { { 0.564705908f, 0.933333397f, 0.564705908f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightGray = { { { 0.827451050f, 0.827451050f, 0.827451050f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightPink = { { { 1.000000000f, 0.713725507f, 0.756862819f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightSalmon = { { { 1.000000000f, 0.627451003f, 0.478431404f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightSeaGreen = { { { 0.125490203f, 0.698039234f, 0.666666687f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightSkyBlue = { { { 0.529411793f, 0.807843208f, 0.980392218f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightSlateGray = { { { 0.466666698f, 0.533333361f, 0.600000024f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightSteelBlue = { { { 0.690196097f, 0.768627524f, 0.870588303f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LightYellow = { { { 1.000000000f, 1.000000000f, 0.878431439f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Lime = { { { 0.000000000f, 1.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 LimeGreen = { { { 0.196078449f, 0.803921640f, 0.196078449f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Linen = { { { 0.980392218f, 0.941176534f, 0.901960850f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Magenta = { { { 1.000000000f, 0.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Maroon = { { { 0.501960814f, 0.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumAquamarine = { { { 0.400000036f, 0.803921640f, 0.666666687f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumBlue = { { { 0.000000000f, 0.000000000f, 0.803921640f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumOrchid = { { { 0.729411781f, 0.333333343f, 0.827451050f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumPurple = { { { 0.576470613f, 0.439215720f, 0.858823597f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumSeaGreen = { { { 0.235294133f, 0.701960802f, 0.443137288f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumSlateBlue = { { { 0.482352972f, 0.407843173f, 0.933333397f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumSpringGreen = { { { 0.000000000f, 0.980392218f, 0.603921592f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumTurquoise = { { { 0.282352954f, 0.819607913f, 0.800000072f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MediumVioletRed = { { { 0.780392230f, 0.082352944f, 0.521568656f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MidnightBlue = { { { 0.098039225f, 0.098039225f, 0.439215720f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MintCream = { { { 0.960784376f, 1.000000000f, 0.980392218f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 MistyRose = { { { 1.000000000f, 0.894117713f, 0.882353008f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Moccasin = { { { 1.000000000f, 0.894117713f, 0.709803939f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 NavajoWhite = { { { 1.000000000f, 0.870588303f, 0.678431392f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Navy = { { { 0.000000000f, 0.000000000f, 0.501960814f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 OldLace = { { { 0.992156923f, 0.960784376f, 0.901960850f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Olive = { { { 0.501960814f, 0.501960814f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 OliveDrab = { { { 0.419607878f, 0.556862772f, 0.137254909f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Orange = { { { 1.000000000f, 0.647058845f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 OrangeRed = { { { 1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Orchid = { { { 0.854902029f, 0.439215720f, 0.839215755f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PaleGoldenrod = { { { 0.933333397f, 0.909803987f, 0.666666687f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PaleGreen = { { { 0.596078455f, 0.984313786f, 0.596078455f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PaleTurquoise = { { { 0.686274529f, 0.933333397f, 0.933333397f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PaleVioletRed = { { { 0.858823597f, 0.439215720f, 0.576470613f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PapayaWhip = { { { 1.000000000f, 0.937254965f, 0.835294187f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PeachPuff = { { { 1.000000000f, 0.854902029f, 0.725490212f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Peru = { { { 0.803921640f, 0.521568656f, 0.247058839f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Pink = { { { 1.000000000f, 0.752941251f, 0.796078503f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Plum = { { { 0.866666734f, 0.627451003f, 0.866666734f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 PowderBlue = { { { 0.690196097f, 0.878431439f, 0.901960850f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Purple = { { { 0.501960814f, 0.000000000f, 0.501960814f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Red = { { { 1.000000000f, 0.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 RosyBrown = { { { 0.737254918f, 0.560784340f, 0.560784340f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 RoyalBlue = { { { 0.254901975f, 0.411764741f, 0.882353008f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SaddleBrown = { { { 0.545098066f, 0.270588249f, 0.074509807f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Salmon = { { { 0.980392218f, 0.501960814f, 0.447058856f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SandyBrown = { { { 0.956862807f, 0.643137276f, 0.376470625f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SeaGreen = { { { 0.180392161f, 0.545098066f, 0.341176480f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SeaShell = { { { 1.000000000f, 0.960784376f, 0.933333397f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Sienna = { { { 0.627451003f, 0.321568638f, 0.176470593f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Silver = { { { 0.752941251f, 0.752941251f, 0.752941251f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SkyBlue = { { { 0.529411793f, 0.807843208f, 0.921568692f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SlateBlue = { { { 0.415686309f, 0.352941185f, 0.803921640f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SlateGray = { { { 0.439215720f, 0.501960814f, 0.564705908f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Snow = { { { 1.000000000f, 0.980392218f, 0.980392218f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SpringGreen = { { { 0.000000000f, 1.000000000f, 0.498039246f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 SteelBlue = { { { 0.274509817f, 0.509803951f, 0.705882370f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Tan = { { { 0.823529482f, 0.705882370f, 0.549019635f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Teal = { { { 0.000000000f, 0.501960814f, 0.501960814f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Thistle = { { { 0.847058892f, 0.749019623f, 0.847058892f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Tomato = { { { 1.000000000f, 0.388235331f, 0.278431386f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Transparent = { { { 0.000000000f, 0.000000000f, 0.000000000f, 0.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Turquoise = { { { 0.250980407f, 0.878431439f, 0.815686345f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Violet = { { { 0.933333397f, 0.509803951f, 0.933333397f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Wheat = { { { 0.960784376f, 0.870588303f, 0.701960802f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 White = { { { 1.000000000f, 1.000000000f, 1.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 WhiteSmoke = { { { 0.960784376f, 0.960784376f, 0.960784376f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 Yellow = { { { 1.000000000f, 1.000000000f, 0.000000000f, 1.000000000f } } };
    XMGLOBALCONST XMVECTORF32 YellowGreen = { { { 0.603921592f, 0.803921640f, 0.196078449f, 1.000000000f } } };

} // namespace colors