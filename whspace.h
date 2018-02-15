#pragma once

bool whspace( char chr )
{
	return chr == '\r' || chr == '\n' || chr == '\t' || chr == ' ';
}