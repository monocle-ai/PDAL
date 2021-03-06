#include <pdal/pdal_test_main.hpp>

#include "Support.hpp"

#include <pdal/PipelineManager.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/PointView.hpp>
#include <pdal/util/FileUtils.hpp>

#include <io/LasReader.hpp>
#include <io/LasWriter.hpp>
#include "../io/SlpkReader.hpp"

using namespace pdal;
//
//test small autzen slpk data provided by esri
TEST(SlpkReaderTest, SlpkReaderTest_read_local)
{

    StageFactory f;
    //create args
    Options slpk_options;
    slpk_options.add("filename", Support::datapath("i3s/SMALL_AUTZEN_LAS_All.slpk"));
    slpk_options.add("threads", 64);
    slpk_options.add("dimensions", "intensity, returns");

    SlpkReader reader;
    reader.setOptions(slpk_options);

    PointTable table;
    reader.prepare(table);

    PointViewSet viewSet = reader.execute(table);
    PointViewPtr view = *viewSet.begin();

    EXPECT_EQ(view->size(), 106u);
    ASSERT_TRUE(table.layout()->hasDim(Dimension::Id::Intensity));
    ASSERT_TRUE(table.layout()->hasDim(Dimension::Id::NumberOfReturns));
    ASSERT_FALSE(table.layout()->hasDim(Dimension::Id::GpsTime));
}


TEST(SlpkReaderTest, slpkReaderTest_bounded)
{
    StageFactory f;
    //create args
    Options slpk_options;
    BOX3D bounds(-123.077, 44.053, 130, -123.063, 44.06, 175);

    slpk_options.add("filename",
        Support::datapath("i3s/SMALL_AUTZEN_LAS_All.slpk"));
    slpk_options.add("threads", 64);

    SlpkReader reader;
    reader.setOptions(slpk_options);

    PointTable table;
    reader.prepare(table);

    PointViewSet viewSet = reader.execute(table);
    PointViewPtr view = *viewSet.begin();
    EXPECT_EQ(view->size(), 106u);

    Options slpk2_options;
    slpk2_options.add("filename",
        Support::datapath("i3s/SMALL_AUTZEN_LAS_All.slpk"));
    slpk2_options.add("threads", 64);
    slpk2_options.add("bounds", Bounds(bounds));

    SlpkReader reader2;
    reader2.setOptions(slpk2_options);

    PointTable table2;
    reader2.prepare(table2);
    PointViewSet viewSet2 = reader2.execute(table2);
    PointViewPtr view2 = *viewSet2.begin();
    EXPECT_EQ(view2->size(), 24u);

    double x, y, z;
    uint64_t count = 0;
    // Count the number of points in the full result that
    for (std::size_t i = 0; i < view->size(); i++)
    {
        x = view->getFieldAs<double>(Dimension::Id::X, i);
        y = view->getFieldAs<double>(Dimension::Id::Y, i);
        z = view->getFieldAs<double>(Dimension::Id::Z, i);
        if (bounds.contains(x,y,z))
            count++;
    }

    // Make sure all points in the filtered view are in the bounds
    // we filtered on.
    for (std::size_t i = 0; i < view2->size(); i++)
    {
        x = view2->getFieldAs<double>(Dimension::Id::X, i);
        y = view2->getFieldAs<double>(Dimension::Id::Y, i);
        z = view2->getFieldAs<double>(Dimension::Id::Z, i);
        ASSERT_TRUE(bounds.contains(x,y,z));
    }
    EXPECT_EQ(view2->size(), count);
}
