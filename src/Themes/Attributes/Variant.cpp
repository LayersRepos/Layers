#include "../../../include/Attribute.h"

using Layers::Variant;

Variant::Variant() :
	m_qvariant{ QVariant() } { }

Variant::Variant(double d) :
	m_qvariant{ QVariant(d) } { }

Variant::Variant(QColor color) :
	m_qvariant{ QVariant(color) } { }

Variant::Variant(QVariant qvariant) :
	m_qvariant{ qvariant } { }

Variant::Variant(const Variant& variant) {
	if (this != &variant)
		*this = variant;
}

void Variant::operator=(const Variant& variant) {
	m_qvariant = variant.m_qvariant;

	emit changed();
}

void Variant::operator=(const QVariant& qvariant) {
	m_qvariant = qvariant;

	emit changed();
}

bool Variant::operator!=(const QVariant& qvariant) {
	return m_qvariant != qvariant;
}

const char* Variant::typeName() const {
	return m_qvariant.typeName();
}
