#include "jadb_header.h"

using namespace jadb;

namespace jadb
{
	struct HeaderData
	{
		uint32_t data_begin;
		uint32_t data_end;
		uint32_t data_rows_count;

		static const size_t Length = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
	};
}

Header::Header(DataFile& owner)
	: m_ref(owner), m_dirty(false)
{
	if (m_ref.size() < length())
	{
		m_dirty = true;
		update();
	}
	read();
}

size_t Header::length()
{
	return HeaderData::Length;
}

uint32_t Header::begin() const
{
	return m_data->data_begin;
}

uint32_t Header::end() const
{
	return m_data->data_end;
}

uint32_t Header::rows() const
{
	return m_data->data_rows_count;
}

Header::~Header()
{
	if (m_data != nullptr)
	{
		delete m_data;
	}
	update();
}

void Header::read()
{
	if (m_data != nullptr)
		delete m_data;
	
	m_data = new HeaderData;
	
	uint8_t data[HeaderData::Length];
	size_t pos = m_ref.pos();

	m_ref.seek(0);
	m_ref.read(data, HeaderData::Length);
	
	size_t offset = 0;
	m_data->data_begin = *(data + offset);
	
	offset += sizeof(m_data->data_begin);
	m_data->data_end = *(data + offset);
	
	offset += sizeof(m_data->data_end);
	m_data->data_rows_count = *(data + offset);
	
	m_ref.seek(pos);
	m_dirty = false;
}

void Header::update()
{
	if (!m_dirty)
		return;

	if (m_data == nullptr)
	{
		m_data = new HeaderData();
		m_data->data_begin = 0;
		m_data->data_end = 0;
		m_data->data_rows_count = 0;
	}

	uint8_t data[HeaderData::Length];
	size_t pos = m_ref.pos();

	m_ref.seek(0);

	size_t offset = 0;

	*(data + offset) = m_data->data_begin;
	offset += sizeof(m_data->data_begin);

	*(data + offset) = m_data->data_end;
	offset += sizeof(m_data->data_end);

	*(data + offset) = m_data->data_rows_count;

	m_ref.write(data, HeaderData::Length);

	m_ref.seek(pos);
	m_dirty = false;
	m_ref.flush();
}
