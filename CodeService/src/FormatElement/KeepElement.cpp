#include "CodeService/FormatElement/KeepElement.h"
#include "Util/format.h"

KeepElement::KeepElement(int keepBlank, bool hasLinebreak)
	: _keepBlank(keepBlank),
	  _hasLinebreak(hasLinebreak)
{
}

FormatElementType KeepElement::GetType()
{
	return FormatElementType::KeepElement;
}

void KeepElement::Serialize(FormatContext& ctx, int position, FormatElement& parent)
{
	int lastElementLine = getLastValidLine(ctx, position, parent);
	int nextElementLine = getNextValidLine(ctx, position, parent);

	if (nextElementLine == -1)
	{
		return;
	}
	// 这个条件的意思是如果上一个元素和下一个元素没有实质的换行则保持一定的空格
	if (nextElementLine == lastElementLine && ctx.GetCharacterCount() != 0)
	{
		ctx.PrintBlank(_keepBlank);
	}
	else
	{
		int line = nextElementLine - lastElementLine;
		if (_hasLinebreak)
		{
			line--;
		}
		ctx.PrintLine(line);
	}
}

void KeepElement::Diagnosis(FormatContext& ctx, int position, FormatElement& parent)
{
	int lastElementLine = getLastValidLine(ctx, position, parent);
	int nextElementLine = getNextValidLine(ctx, position, parent);

	if (nextElementLine == -1)
	{
		return;
	}

	if (nextElementLine == lastElementLine)
	{
		int lastOffset = getLastValidOffset(ctx, position, parent);
		int nextOffset = getNextValidOffset(ctx, position, parent);
		if (nextOffset - lastOffset - 1 != _keepBlank)
		{
			ctx.PushDiagnosis(format("must keep {} space", _keepBlank), TextRange(lastOffset, nextOffset));
		}
	}
}
