#pragma once

//ExportName: BlendMode

#include "global.h"

#include <QString>
#include <QPainter>
#include <QHash>
#include <QMetaType>
#include "blendop.h"

namespace Malachite
{

class MALACHITESHARED_EXPORT BlendMode
{
public:
	
	/**
	  This enum describes the blend mode (aka compositing mode) in painting.
	  Malachite supports all compositing operations of SVG Compositing Specification (http://www.w3.org/TR/SVGCompositing/) and some original modes.
	*/
	enum Index
	{
		/**
		  Normal (通常)
		  Equal to BlendModeSourceover
		*/
		Normal = 0,
		
		/**
		  Plus (加算)
		*/
		Plus,
		
		/**
		  Multiply (乗算)
		*/
		Multiply,
		
		/**
		  Screen (スクリーン)
		*/
		Screen,
		
		/**
		  Overlay (オーバーレイ)
		*/
		Overlay,
		
		/**
		  Darken (比較（暗）)
		*/
		Darken,
		
		/**
		  Lighten (比較（明）)
		*/
		Lighten,
		
		/**
		  Color Dodge (覆い焼き)
		*/
		ColorDodge,
		
		/**
		  Color Burn (焼き込み)
		*/
		ColorBurn,
		
		/**
		  Hard Light (ハードライト)
		*/
		HardLight,
		
		/**
		  Soft Light (ソフトライト)
		*/
		SoftLight,
		
		/**
		  Difference (差の絶対値)
		*/
		Difference,
		
		/**
		  Exclusion (除外)
		*/
		Exclusion,
		
		Hue,
		Saturation,
		Color,
		Luminosity,
		
		Clear = 128,
		Source,
		Destination,
		SourceOver,
		DestinationOver,
		SourceIn,
		DestinationIn,
		SourceOut,
		DestinationOut,
		SourceAtop,
		DestinationAtop,
		Xor,
		
		/**
		  Pass Through
		  This value is only for group layers.
		  This value means that the group layer will not affect layer blending order.
		*/
		PassThrough = 256
	};
	
	BlendMode() : _index(Normal) {}
	BlendMode(int index) : _index(index) {}
	BlendMode(const QString &modeName) : _index(_dict.indexForName(modeName)) {}
	
	int toInt() const { return _index; }
	void setInt(int index) { _index = index; }
	
	QString toString() const { return _dict.name(_index); }
	void setString(const QString &name) { _index = _dict.indexForName(name); }
	
	QPainter::CompositionMode toQPainterMode() const { return _dict.qPainterMode(_index); }
	
	BlendOp *op() const { return blendOpDictionary()->blendOp(_index); }
	
	BlendMode &operator=(int index)
	{
		_index = index;
		return *this;
	}
	
	bool operator==(int index) { return _index == index; }
	bool operator!=(int index) { return _index != index; }
	
private:
	
	class Dictionary
	{
	public:
		Dictionary();
		
		QString name(int index) const
			{ return nameHash.value(index, "normal"); }
		QPainter::CompositionMode qPainterMode(int index) const
			{ return qtCompositionModeHash.value(index, QPainter::CompositionMode_SourceOver); }
		int indexForName(const QString &name) const
			{ return nameHash.key(name, SourceOver); }
		
	private:
		QHash<int, QString> nameHash;
		QHash<int, QPainter::CompositionMode> qtCompositionModeHash;
	};
	
	static Dictionary _dict;
	
	int _index;
};

}
