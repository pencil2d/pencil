/*

Pencil - Traditional Animation Software
Copyright (C) 2012-2018 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, char* argv[])
{
    int result = Catch::Session().run(argc, argv);
    return result;
}
