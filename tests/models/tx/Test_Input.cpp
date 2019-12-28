#include <catch.hpp>

#include <mw/core/models/tx/Input.h>

TEST_CASE("Input")
{
	EOutputFeatures feature = EOutputFeatures::DEFAULT_OUTPUT;
	Commitment commit = Commitment::FromHex("000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20");

	Input input(feature, std::move(commit));

	const json json = input.ToJSON();
	Input input2 = Input::FromJSON(json);

	REQUIRE(input == input2);
	// TODO: Finish this
}