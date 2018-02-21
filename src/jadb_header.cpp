#include "jadb_header.h"

using namespace jadb;

Header::Header()
{}

void Header::setRows(uint32_t rows)
{
	m_rows = rows;
}

uint32_t Header::begin() const
{
	return m_data_begin;
}

uint32_t Header::end() const
{
	return m_data_end;
}

uint32_t Header::rows() const
{
	return m_rows;
}

Header::~Header()
{
}


