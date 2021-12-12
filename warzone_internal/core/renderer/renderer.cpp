#include "renderer.h"
#include <cmath>
#include <iostream>

void get_dec_str(unsigned char* str, size_t len, unsigned int val) {
	unsigned char i;
	for (i = 1; i <= len; i++)
	{
		str[len - i] = (unsigned char)((val % 10UL) + '0');
		val /= 10;
	}
}

HMODULE get_compiler() {
	char gConstructedString[] = "D3DCompiler_%i.dll";
	for (int i = 30; i < 50; i++)
	{
		//sprintf(Temp, gConstructedString, i); // # at index 12 and 13
		unsigned char* Temp2 = (unsigned char*)(gConstructedString + 12); // sprintf replacement to narrow down imports
		get_dec_str(Temp2, 2, i);
		if (GetModuleHandleA(gConstructedString)) return GetModuleHandleA(gConstructedString);
	}
	return NULL;
}

renderer::renderer(IDXGISwapChain* swapchain, ID3D11Device* direct_3d_device, const std::wstring& default_font_family) :
	direct_3d_device(direct_3d_device),
	immediate_context(nullptr),
	input_layout(nullptr),
	vertex_shader(nullptr),
	pixel_shader(nullptr),
	font_factory(nullptr),
	font_wrapper(nullptr),
	default_font_family(default_font_family),
	max_vertices(1024 * 4 * 3)
{
	auto d3d_compile = (d3d_compile_t)GetProcAddress(get_compiler(), "D3DCompile");

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 16 ,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3DBlob* vs_blob = nullptr;
	ID3DBlob* ps_blob = nullptr;

	direct_3d_device->GetImmediateContext(&immediate_context);

	throw_if_failed(FW1CreateFactory(FW1_VERSION, &font_factory));

	render_list_ptr = new render_list(font_factory, max_vertices);

	throw_if_failed(font_factory->CreateFontWrapper(direct_3d_device, default_font_family.c_str(), &font_wrapper));

	throw_if_failed(d3d_compile(shader, std::size(shader), nullptr, nullptr, nullptr, "VS", "vs_4_0", 0, 0, &vs_blob, nullptr));
	throw_if_failed(d3d_compile(shader, std::size(shader), nullptr, nullptr, nullptr, "PS", "ps_4_0", 0, 0, &ps_blob, nullptr));

	throw_if_failed(direct_3d_device->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), nullptr, &vertex_shader));
	throw_if_failed(direct_3d_device->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(), nullptr, &pixel_shader));

	throw_if_failed(direct_3d_device->CreateInputLayout(layout, static_cast<UINT>(std::size(layout)), vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(), &input_layout));

	safe_release(vs_blob);
	safe_release(ps_blob);

	D3D11_BLEND_DESC blend_desc{};

	blend_desc.RenderTarget->BlendEnable = TRUE;
	blend_desc.RenderTarget->SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_desc.RenderTarget->DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_desc.RenderTarget->SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget->DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget->BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	throw_if_failed(direct_3d_device->CreateBlendState(&blend_desc, &blend_state));

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.ByteWidth = sizeof(vertex) * static_cast<UINT>(max_vertices);
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;

	throw_if_failed(direct_3d_device->CreateBuffer(&buffer_desc, nullptr, &vertex_buffer));

	buffer_desc = {};
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.ByteWidth = sizeof(XMMATRIX);
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;

	throw_if_failed(direct_3d_device->CreateBuffer(&buffer_desc, nullptr, &screen_projection_buffer));

	DXGI_SWAP_CHAIN_DESC swap_chain_desc;
	memset(&swap_chain_desc, 0, sizeof(swap_chain_desc));
	swapchain->GetDesc(&swap_chain_desc);

	D3D11_VIEWPORT viewport;
	UINT viewportnumber = 1;
	immediate_context->RSGetViewports(&viewportnumber, &viewport);
	viewport.Width = swap_chain_desc.BufferDesc.Width; 
	viewport.Height = swap_chain_desc.BufferDesc.Height; 
	width = swap_chain_desc.BufferDesc.Width;
	height = swap_chain_desc.BufferDesc.Height;
	immediate_context->RSSetViewports(viewportnumber, &viewport);

	projection = XMMatrixOrthographicOffCenterLH(viewport.TopLeftX, viewport.Width, viewport.Height, viewport.TopLeftY, viewport.MinDepth, viewport.MaxDepth);

	D3D11_MAPPED_SUBRESOURCE mapped_resource;
	throw_if_failed(immediate_context->Map(screen_projection_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource)); {
		std::memcpy(mapped_resource.pData, &projection, sizeof(XMMATRIX));
	}
	immediate_context->Unmap(screen_projection_buffer, 0);
}

renderer::~renderer() {
	safe_release(vertex_shader);
	safe_release(pixel_shader);
	safe_release(vertex_buffer);
	safe_release(screen_projection_buffer);
	safe_release(input_layout);
	safe_release(blend_state);
	safe_release(font_wrapper);
	safe_release(font_factory);
}

void renderer::begin() {
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	immediate_context->OMSetBlendState(blend_state, nullptr, 0xffffffff);
	immediate_context->VSSetConstantBuffers(0, 1, &screen_projection_buffer);

	immediate_context->IASetInputLayout(input_layout);

	UINT stride = sizeof(vertex);
	UINT offset = 0;
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	font_wrapper->DrawString(immediate_context, L"", 0.0f, 0.0f, 0.0f, 0xff000000, FW1_RESTORESTATE | FW1_NOFLUSH);
}

void renderer::end() {
	render_list_ptr->clear();
}

void renderer::draw(const render_list* renderList) {
	if (std::size(render_list_ptr->vertices) > 0)
	{
		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		throw_if_failed(immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource));
		{
			std::memcpy(mapped_resource.pData, renderList->vertices.data(), sizeof(vertex) * std::size(render_list_ptr->vertices));
		}
		immediate_context->Unmap(vertex_buffer, 0);
	}

	std::size_t pos = 0;

	for (const auto& batch : renderList->batches)
	{
		immediate_context->IASetPrimitiveTopology(batch.topology);
		immediate_context->Draw(static_cast<UINT>(batch.count), static_cast<UINT>(pos));

		pos += batch.count;
	}

	font_wrapper->Flush(immediate_context);
	font_wrapper->DrawGeometry(immediate_context, renderList->text_geometry, nullptr, nullptr, FW1_RESTORESTATE);

}

void renderer::draw() {
	draw(render_list_ptr);
}

void renderer::add_vertex(const render_list* renderList, vertex& vertex, D3D11_PRIMITIVE_TOPOLOGY topology) {
	assert(topology != D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
		&& "add_vertex >Use add_vertices to draw line/triangle strips!");
	assert(topology != D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ
		&& "add_vertex >Use add_vertices to draw line/triangle strips!");
	assert(topology != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
		&& "add_vertex >Use add_vertices to draw line/triangle strips!");
	assert(topology != D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ
		&& "add_vertex >Use add_vertices to draw line/triangle strips!");

	if (std::size(render_list_ptr->vertices) >= max_vertices)
		(this->render_list_ptr == renderList) ? draw(render_list_ptr) : throw std::exception(
			"Renderer::add_vertex - vertex buffer exhausted! Increase the size of the vertex buffer or add a custom implementation.");

	if (std::empty(render_list_ptr->batches) || renderList->batches.back().topology != topology)
		renderList->batches.emplace_back(0, topology);

	renderList->batches.back().count += 1;
	renderList->vertices.push_back(vertex);
}

void renderer::add_vertex(vertex& vertex, D3D11_PRIMITIVE_TOPOLOGY topology) {
	add_vertex(render_list_ptr, vertex, topology);
}

void renderer::draw_text(const render_list* renderList, const vec2_t& pos, const std::wstring& text, const color& color, std::uint32_t flags, float fontSize, const std::wstring& fontFamily) {
	std::uint32_t transformedcolor = XMCOLOR(color.f[2], color.f[1], color.f[0], color.f[3]);

	FW1_RECTF rect = { pos.x, pos.y, pos.x, pos.y };
	font_wrapper->AnalyzeString(nullptr, text.c_str(), (fontFamily == std::wstring{}) ? default_font_family.c_str() :
		fontFamily.c_str(), fontSize, &rect, transformedcolor, flags | FW1_NOFLUSH | FW1_NOWORDWRAP, renderList->text_geometry);
}

void renderer::draw_text(const vec2_t& pos, const std::wstring& text, const color& color, std::uint32_t flags, float fontSize, const std::wstring& fontFamily) {
	draw_text(render_list_ptr, pos, text, color, flags, fontSize, fontFamily);
}

vec2_t renderer::get_text_extent(const std::wstring& text, float fontSize, const std::wstring& fontFamily) const {
	FW1_RECTF nullRect = { 0.f, 0.f, 0.f, 0.f };
	FW1_RECTF rect = font_wrapper->MeasureString(text.c_str(), (fontFamily == std::wstring{}) ? default_font_family.c_str() : fontFamily.c_str(),
		fontSize, &nullRect, FW1_NOWORDWRAP);
	return{ rect.Right, rect.Bottom };
}

void renderer::draw_pixel(const render_list* renderList, const vec2_t& pos, const color& color) {
	draw_filled_rect(render_list_ptr, XMFLOAT4{ pos.x, pos.y, 1.f, 1.f }, color);
}

void renderer::draw_pixel(const vec2_t& pos, const color& color) {
	draw_filled_rect(render_list_ptr, XMFLOAT4{ pos.x, pos.y, 1.f, 1.f }, color);
}

void renderer::draw_line(const render_list* renderList, const vec2_t& from, const vec2_t& to, const color& color) {
	vertex v[]
	{
		{ from.x,	from.y,	0.f, color },
		{ to.x,		to.y,	0.f, color }
	};

	add_vertices(render_list_ptr, v, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void renderer::draw_line(const vec2_t& from, const vec2_t& to, const color& color) {
	draw_line(render_list_ptr, from, to, color);
}

void renderer::draw_filled_rect(const render_list* renderList, const vec4_t& rect, const color& color) {
	vertex v[]
	{
		{ rect.x,			rect.y,				0.f, color },
		{ rect.x + rect.z,	rect.y,				0.f, color },
		{ rect.x,			rect.y + rect.w,	0.f, color },

		{ rect.x + rect.z,	rect.y,				0.f, color },
		{ rect.x + rect.z,	rect.y + rect.w,	0.f, color },
		{ rect.x,			rect.y + rect.w,	0.f, color }
	};

	add_vertices(render_list_ptr, v, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void renderer::draw_filled_rect(const vec4_t& rect, const color& color) {
	draw_filled_rect(render_list_ptr, rect, color);
}

void renderer::draw_rect(const render_list* renderList, const vec4_t& rect, float strokeWidth, const color& color) {
	XMFLOAT4 tmp = rect;
	tmp.z = strokeWidth;
	draw_filled_rect(render_list_ptr, tmp, color);
	tmp.x = rect.x + rect.z - strokeWidth;
	draw_filled_rect(render_list_ptr, tmp, color);
	tmp.z = rect.z;
	tmp.x = rect.x;
	tmp.w = strokeWidth;
	draw_filled_rect(render_list_ptr, tmp, color);
	tmp.y = rect.y + rect.w;
	draw_filled_rect(render_list_ptr, tmp, color);
}

void renderer::draw_rect(const vec4_t& rect, float strokeWidth, const color& color) {
	draw_rect(render_list_ptr, rect, strokeWidth, color);
}

void renderer::draw_outlined_rect(const render_list* renderList, const vec4_t& rect, float strokeWidth, const color& strokecolor, const color& fillcolor) {
	draw_filled_rect(render_list_ptr, rect, fillcolor);
	draw_rect(render_list_ptr, rect, strokeWidth, strokecolor);
}

void renderer::draw_outlined_rect(const vec4_t& rect, float strokeWidth, const color& strokecolor, const color& fillcolor) {
	draw_outlined_rect(render_list_ptr, rect, strokeWidth, strokecolor, fillcolor);
}

void renderer::draw_circle(const render_list* renderList, const vec2_t& pos, float radius, const color& color) {
	const int segments = 24;

	vertex v[segments + 1];

	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.f * XM_PI * static_cast<float>(i) / static_cast<float>(segments);

		v[i] = vertex{
			pos.x + radius * std::cos(theta),
			pos.y + radius * std::sin(theta),
			0.f, color
		};
	}

	add_vertices(render_list_ptr, v, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void renderer::draw_circle(const vec2_t& pos, float radius, const color& color) {
	draw_circle(render_list_ptr, pos, radius, color);
}

std::shared_ptr<renderer> renderer::ptr()
{
	return shared_from_this();
}

IFW1Factory* renderer::get_font_factory() const {
	return font_factory;
}