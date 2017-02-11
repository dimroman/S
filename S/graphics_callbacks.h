#include <functional>

namespace math
{
	struct float4;
}

using selection_updated_callback_type = std::function<void(bool const, bool const, math::float4&)>;

void empty_selection_updated_callback(bool const is_selected, bool const is_highlighted, math::float4& color);
void default_selection_updated_callback(bool const is_selected, bool const is_highlighted, math::float4& color);