/* xoreos-tools - Tools to help with xoreos development
 *
 * xoreos-tools is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos-tools is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos-tools. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Unit tests for our Nintendo DS compression.
 */

#include "gtest/gtest.h"

#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/smallfile.h"

// Percy Bysshe Shelley's "Ozymandias"
static const char *kDataUncompressed =
	"I met a traveller from an antique land\n"
	"Who said: Two vast and trunkless legs of stone\n"
	"Stand in the desert. Near them, on the sand,\n"
	"Half sunk, a shattered visage lies, whose frown,\n"
	"And wrinkled lip, and sneer of cold command,\n"
	"Tell that its sculptor well those passions read\n"
	"Which yet survive, stamped on these lifeless things,\n"
	"The hand that mocked them and the heart that fed:\n"
	"And on the pedestal these words appear:\n"
	"'My name is Ozymandias, king of kings:\n"
	"Look on my works, ye Mighty, and despair!'\n"
	"Nothing beside remains. Round the decay\n"
	"Of that colossal wreck, boundless and bare\n"
	"The lone and level sands stretch far away.";

// Percy Bysshe Shelley's "Ozymandias", "compressed" using the 0x00 mode
static const byte kDataCompressed00[] =
	"\x00""\x6F""\x02""\x00"
	"I met a traveller from an antique land\n"
	"Who said: Two vast and trunkless legs of stone\n"
	"Stand in the desert. Near them, on the sand,\n"
	"Half sunk, a shattered visage lies, whose frown,\n"
	"And wrinkled lip, and sneer of cold command,\n"
	"Tell that its sculptor well those passions read\n"
	"Which yet survive, stamped on these lifeless things,\n"
	"The hand that mocked them and the heart that fed:\n"
	"And on the pedestal these words appear:\n"
	"'My name is Ozymandias, king of kings:\n"
	"Look on my works, ye Mighty, and despair!'\n"
	"Nothing beside remains. Round the decay\n"
	"Of that colossal wreck, boundless and bare\n"
	"The lone and level sands stretch far away.";

// Percy Bysshe Shelley's "Ozymandias", compressed using the LZSS 0x10 mode
static const byte kDataCompressed10[] = {
	0x10,0x6F,0x02,0x00,0x00,0x49,0x20,0x6D,0x65,0x74,0x20,0x61,0x20,0x00,0x74,0x72,
	0x61,0x76,0x65,0x6C,0x6C,0x65,0x00,0x72,0x20,0x66,0x72,0x6F,0x6D,0x20,0x61,0x40,
	0x6E,0x00,0x02,0x74,0x69,0x71,0x75,0x65,0x20,0x00,0x6C,0x61,0x6E,0x64,0x0A,0x57,
	0x68,0x6F,0x00,0x20,0x73,0x61,0x69,0x64,0x3A,0x20,0x54,0x02,0x77,0x6F,0x20,0x76,
	0x61,0x73,0x00,0x33,0x6E,0x40,0x64,0x00,0x35,0x75,0x6E,0x6B,0x6C,0x65,0x73,0x00,
	0x73,0x20,0x6C,0x65,0x67,0x73,0x20,0x6F,0x00,0x66,0x20,0x73,0x74,0x6F,0x6E,0x65,
	0x0A,0x20,0x53,0x74,0x10,0x1D,0x69,0x6E,0x20,0x74,0x68,0x00,0x65,0x20,0x64,0x65,
	0x73,0x65,0x72,0x74,0x02,0x2E,0x20,0x4E,0x65,0x61,0x72,0x10,0x10,0x6D,0x14,0x2C,
	0x20,0x6F,0x30,0x19,0x73,0x00,0x5A,0x2C,0x0A,0x1A,0x48,0x61,0x6C,0x00,0x37,0x00,
	0x48,0x2C,0x00,0x87,0x73,0x00,0x68,0x61,0x74,0x74,0x65,0x72,0x65,0x64,0x02,0x20,
	0x76,0x69,0x73,0x61,0x67,0x00,0x7E,0x69,0x00,0x65,0x73,0x2C,0x20,0x77,0x68,0x6F,
	0x73,0x41,0x65,0x10,0x9A,0x77,0x6E,0x2C,0x0A,0x41,0x00,0x79,0x15,0x77,0x72,0x69,
	0x10,0x79,0x64,0x00,0x1F,0x70,0x00,0x37,0x8C,0x00,0x8B,0x73,0x6E,0x65,0x00,0xBD,
	0x00,0x82,0x63,0x6F,0x25,0x6C,0x64,0x00,0x04,0x6D,0x6D,0x20,0x5D,0x54,0x00,0xD5,
	0x80,0x00,0x86,0x61,0x74,0x20,0x69,0x74,0x73,0x20,0x00,0x73,0x63,0x75,0x6C,0x70,
	0x74,0x6F,0x72,0x30,0x20,0x77,0x30,0x16,0x10,0x55,0x70,0x61,0x73,0x73,0x00,0x69,
	0x6F,0x6E,0x73,0x20,0x72,0x65,0x61,0x82,0x10,0xE9,0x69,0x63,0x68,0x20,0x79,0x01,
	0x14,0x73,0x01,0x75,0x72,0x76,0x69,0x76,0x65,0x2C,0x00,0xD3,0x1C,0x61,0x6D,0x70,
	0x00,0x91,0x30,0xB5,0x00,0x87,0x6C,0x69,0x20,0x66,0x65,0x20,0xF5,0x74,0x68,0x69,
	0x6E,0x67,0x6C,0x73,0x00,0x64,0x00,0xE6,0x68,0x21,0x10,0x10,0x68,0x6D,0x6F,0x3E,
	0x63,0x6B,0x00,0xC0,0x10,0xEA,0x31,0x25,0x10,0x1D,0x00,0xFC,0x74,0x87,0x30,0x87,
	0x66,0x65,0x64,0x3A,0x20,0xC3,0x41,0x05,0x00,0x5A,0x70,0x65,0x00,0x62,0x10,0xA5,
	0x10,0x5C,0x77,0x6F,0x72,0x64,0x04,0x73,0x20,0x61,0x70,0x70,0x01,0x2E,0x3A,0x0A,
	0x00,0x27,0x4D,0x79,0x20,0x6E,0x61,0x6D,0x65,0x01,0x20,0x69,0x73,0x20,0x4F,0x7A,
	0x79,0x10,0xD3,0x2E,0x69,0x61,0x01,0x10,0x6B,0x00,0x78,0x11,0x6F,0x10,0x07,0x73,
	0x02,0x3A,0x0A,0x4C,0x6F,0x6F,0x6B,0x11,0x55,0x6D,0x50,0x79,0x10,0x3F,0x6B,0x01,
	0x31,0x79,0x65,0x20,0x4D,0x06,0x69,0x67,0x68,0x74,0x79,0x31,0x1D,0x01,0x84,0x70,
	0x00,0x61,0x69,0x72,0x21,0x27,0x0A,0x4E,0x6F,0x80,0x20,0xB5,0x20,0x62,0x65,0x73,
	0x69,0x64,0x65,0x80,0x00,0xF4,0x6D,0x61,0x69,0x6E,0x73,0x2E,0x20,0x10,0x52,0x6F,
	0x75,0x40,0xAB,0x64,0x65,0x63,0x61,0x0C,0x79,0x0A,0x4F,0x66,0x31,0x36,0x01,0x4E,
	0x6F,0x73,0x42,0x73,0x00,0x9E,0x77,0x72,0x65,0x63,0x01,0xA3,0x62,0xE1,0x10,0x27,
	0x21,0xF3,0x12,0x01,0x62,0x61,0x72,0x65,0x20,0xFE,0x67,0x6C,0x01,0xF7,0x22,0x13,
	0x6C,0x65,0x02,0x48,0x21,0xDA,0x01,0x6E,0x0A,0x74,0x72,0x65,0x74,0x01,0x4E,0x66,
	0x01,0xF9,0x61,0x00,0x77,0x61,0x79,0x2E
};

static void compareData(Common::SeekableReadStream &data1, const char *data2) {
	for (size_t i = 0; i < strlen(data2); i++)
		EXPECT_EQ(data1.readByte(), data2[i]) << "At index " << i;
}

static void compareData(const byte *data1, const byte *data2, size_t n) {
	for (size_t i = 0; i < n; i++)
		EXPECT_EQ(data1[i], data2[i]) << "At index " << i;
}

static void compareData(const byte *data1, const char *data2) {
	for (size_t i = 0; i < strlen(data2); i++)
		EXPECT_EQ(data1[i], data2[i]) << "At index " << i;
}

// --- Decompress 0x00 ---

GTEST_TEST(Small0x00, decompress) {
	Common::MemoryWriteStreamDynamic uncompressed(true);
	Common::MemoryReadStream compressed(kDataCompressed00);

	Aurora::Small::decompress(compressed, uncompressed);
	ASSERT_EQ(uncompressed.size(), strlen(kDataUncompressed));

	compareData(uncompressed.getData(), kDataUncompressed);
}

GTEST_TEST(Small0x00, decompressIntoReadStream) {
	Common::MemoryReadStream compressed(kDataCompressed00);

	Common::SeekableReadStream *uncompressed = Aurora::Small::decompress(compressed);
	ASSERT_EQ(uncompressed->size(), strlen(kDataUncompressed));

	compareData(*uncompressed, kDataUncompressed);
	delete uncompressed;
}

GTEST_TEST(Small0x00, decompressIntoReadStreamTakeOverNoSeek) {
	Common::MemoryReadStream *compressed = new Common::MemoryReadStream(kDataCompressed00);

	Common::SeekableReadStream *uncompressed =
		Aurora::Small::decompress(static_cast<Common::ReadStream *>(compressed));
	ASSERT_EQ(uncompressed->size(), strlen(kDataUncompressed));

	compareData(*uncompressed, kDataUncompressed);
	delete uncompressed;
}

GTEST_TEST(Small0x00, decompressIntoReadStreamTakeOverSeek) {
	Common::MemoryReadStream *compressed = new Common::MemoryReadStream(kDataCompressed00);

	Common::SeekableReadStream *uncompressed =
		Aurora::Small::decompress(static_cast<Common::SeekableReadStream *>(compressed));
	ASSERT_EQ(uncompressed->size(), strlen(kDataUncompressed));

	compareData(*uncompressed, kDataUncompressed);
	delete uncompressed;
}

// --- Compress 0x00 ---

GTEST_TEST(Small0x00, compress) {
	Common::MemoryWriteStreamDynamic compressed(true);
	Common::MemoryReadStream uncompressed(kDataUncompressed);

	Aurora::Small::compress00(uncompressed, compressed);
	ASSERT_EQ(compressed.size(), sizeof(kDataCompressed00) - 1);

	compareData(compressed.getData(), kDataCompressed00, sizeof(kDataCompressed00) - 1);
}

GTEST_TEST(Small0x00, compressRoundTrip) {
	Common::MemoryWriteStreamDynamic compressedWrite(true);
	Common::MemoryReadStream uncompressedRead(kDataUncompressed);

	Aurora::Small::compress00(uncompressedRead, compressedWrite);


	Common::MemoryWriteStreamDynamic uncompressedWrite(true);
	Common::MemoryReadStream compressedRead(compressedWrite.getData(), compressedWrite.size());

	Aurora::Small::decompress(compressedRead, uncompressedWrite);


	ASSERT_EQ(uncompressedWrite.size(), strlen(kDataUncompressed));
	compareData(uncompressedWrite.getData(), kDataUncompressed);
}

// --- Decompress 0x10 ---

GTEST_TEST(Small0x10, decompress) {
	Common::MemoryWriteStreamDynamic uncompressed(true);
	Common::MemoryReadStream compressed(kDataCompressed10);

	Aurora::Small::decompress(compressed, uncompressed);
	ASSERT_EQ(uncompressed.size(), strlen(kDataUncompressed));

	compareData(uncompressed.getData(), kDataUncompressed);
}

GTEST_TEST(Small0x10, decompressIntoReadStream) {
	Common::MemoryReadStream compressed(kDataCompressed10);

	Common::SeekableReadStream *uncompressed = Aurora::Small::decompress(compressed);
	ASSERT_EQ(uncompressed->size(), strlen(kDataUncompressed));

	compareData(*uncompressed, kDataUncompressed);
	delete uncompressed;
}

GTEST_TEST(Small0x10, decompressIntoReadStreamTakeOverNoSeek) {
	Common::MemoryReadStream *compressed = new Common::MemoryReadStream(kDataCompressed10);

	Common::SeekableReadStream *uncompressed =
		Aurora::Small::decompress(static_cast<Common::ReadStream *>(compressed));
	ASSERT_EQ(uncompressed->size(), strlen(kDataUncompressed));

	compareData(*uncompressed, kDataUncompressed);
	delete uncompressed;
}

GTEST_TEST(Small0x10, decompressIntoReadStreamTakeOverSeek) {
	Common::MemoryReadStream *compressed = new Common::MemoryReadStream(kDataCompressed10);

	Common::SeekableReadStream *uncompressed =
		Aurora::Small::decompress(static_cast<Common::SeekableReadStream *>(compressed));
	ASSERT_EQ(uncompressed->size(), strlen(kDataUncompressed));

	compareData(*uncompressed, kDataUncompressed);
	delete uncompressed;
}

// --- Compress 0x10 ---

GTEST_TEST(Small0x10, compress) {
	Common::MemoryWriteStreamDynamic compressed(true);
	Common::MemoryReadStream uncompressed(kDataUncompressed);

	Aurora::Small::compress10(uncompressed, compressed);
	ASSERT_EQ(compressed.size(), sizeof(kDataCompressed10));

	compareData(compressed.getData(), kDataCompressed10, sizeof(kDataCompressed10));
}

GTEST_TEST(Small0x10, compressRoundTrip) {
	Common::MemoryWriteStreamDynamic compressedWrite(true);
	Common::MemoryReadStream uncompressedRead(kDataUncompressed);

	Aurora::Small::compress10(uncompressedRead, compressedWrite);


	Common::MemoryWriteStreamDynamic uncompressedWrite(true);
	Common::MemoryReadStream compressedRead(compressedWrite.getData(), compressedWrite.size());

	Aurora::Small::decompress(compressedRead, uncompressedWrite);


	ASSERT_EQ(uncompressedWrite.size(), strlen(kDataUncompressed));
	compareData(uncompressedWrite.getData(), kDataUncompressed);
}