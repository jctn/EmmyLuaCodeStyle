#include "CodeService/FormatElement/AlignmentLayoutElement.h"
#include "CodeService/FormatElement/StatementElement.h"
#include "CodeService/FormatElement/TextElement.h"
#include "CodeService/FormatElement/AlignmentElement.h"

int FindLastValidChildIndex(int position, std::vector<std::shared_ptr<FormatElement>>& vec)
{
	for (int index = position - 1; position >= 0; index--)
	{
		if (vec[index]->HasValidTextRange())
		{
			return index;
		}
	}
	return -1;
}


FormatElementType AlignmentLayoutElement::GetType()
{
	return FormatElementType::AlignmentLayoutElement;
}

void AlignmentLayoutElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	auto eqPosition = getAlignPosition(ctx.GetParser());

	if (eqPosition != -1)
	{
		return alignmentSerialize(ctx, position, parent, eqPosition);
	}
	else
	{
		FormatElement::Serialize(ctx, position, parent);
	}
}

void AlignmentLayoutElement::Diagnosis(DiagnosisContext& ctx, int position, FormatElement& parent)
{
	auto eqPosition = getAlignPosition(ctx.GetParser());

	if (eqPosition != -1)
	{
		return alignmentDiagnosis(ctx, position, parent, eqPosition);
	}
	else
	{
		FormatElement::Diagnosis(ctx, position, parent);
	}
}

int AlignmentLayoutElement::getAlignPosition(std::shared_ptr<LuaParser> luaParser)
{
	int eqAlignedPosition = 0;
	bool firstContainEq = true;
	// 先寻找等号对齐的位置，并且判断连续的带等号的语句是否应该对齐到等号
	// 连续的带等号的语句是否应该对齐到等号，这个行为应该由连续语句的首行决定
	// 如果被子节点内的其他语句共同决定则很难将连续对齐还原为普通排版
	for (int statIndex = 0; statIndex != static_cast<int>(_children.size()); statIndex++)
	{
		auto statChild = _children[statIndex];

		auto& statChildren = statChild->GetChildren();

		for (int index = 0; index != static_cast<int>(statChildren.size()); index++)
		{
			auto textChild = statChildren[index];
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				if (textElement->GetText() == "=")
				{
					int eqPosition = luaParser->GetColumn(textElement->GetTextRange().StartOffset);
					if (firstContainEq && index > 0)
					{
						firstContainEq = false;
						auto lastValidIndex = FindLastValidChildIndex(index, statChildren);
						if (lastValidIndex == -1)
						{
							return -1;
						}
						auto lastStatChild = statChildren[lastValidIndex];
						auto lastPosition = luaParser->GetColumn(lastStatChild->GetTextRange().EndOffset);


						if (eqPosition - lastPosition <= 2)
						{
							return -1;
						}
					}

					if (eqAlignedPosition < eqPosition)
					{
						eqAlignedPosition = eqPosition;
					}
				}
			}
		}
	}

	return eqAlignedPosition;
}

void AlignmentLayoutElement::alignmentSerialize(FormatContext& ctx, int position, FormatElement& parent, int eqPosition)
{
	for (auto statChild : _children)
	{
		auto& statChildren = statChild->GetChildren();

		for (std::size_t i = 0; i < statChildren.size(); i++)
		{
			auto textChild = statChildren[i];
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				if (textElement->GetText() == "=" && i > 0)
				{
					// 将控制元素变更为对齐元素
					statChildren[i - 1] = std::make_shared<AlignmentElement>(eqPosition);
					break;
				}
			}
		}
	}

	return FormatElement::Serialize(ctx, position, parent);
}

void AlignmentLayoutElement::alignmentDiagnosis(DiagnosisContext& ctx, int position, FormatElement& parent,
                                                int eqPosition)
{
	for (auto statChild : _children)
	{
		auto& statChildren = statChild->GetChildren();

		for (std::size_t i = 0; i < statChildren.size(); i++)
		{
			auto textChild = statChildren[i];
			if (textChild->GetType() == FormatElementType::TextElement)
			{
				auto textElement = std::dynamic_pointer_cast<TextElement>(textChild);
				if (textElement->GetText() == "=" && i > 0)
				{
					// 将控制元素变更为对齐元素
					statChildren[i - 1] = std::make_shared<AlignmentElement>(eqPosition);
					break;
				}
			}
		}
	}

	return FormatElement::Diagnosis(ctx, position, parent);
}
