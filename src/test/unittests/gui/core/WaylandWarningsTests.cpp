/*
 * synergy -- mouse and keyboard sharing utility
 * Copyright (C) 2024 Symless Ltd.
 *
 * This package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * found in the file LICENSE that should have accompanied this file.
 *
 * This package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gui/core/WaylandWarnings.h"

#include "gui/core/CoreProcess.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace synergy::gui;
using namespace synergy::gui::core;

namespace {
struct MockDeps : public WaylandWarnings::Deps {
  MOCK_METHOD(void, showWaylandExperimental, (QWidget *), (override));
  MOCK_METHOD(void, showNoEiSupport, (QWidget *), (override));
  MOCK_METHOD(void, showNoPortalSupport, (QWidget *), (override));
  MOCK_METHOD(void, showNoPortalInputCapture, (QWidget *), (override));
};

} // namespace

TEST(WaylandWarningsTests, showOnce_serverNoEiNoPortalIc_showNoEiWarning) {
  const auto deps = std::make_shared<MockDeps>();
  const bool hasEi = false;
  const bool hasPortal = false;
  const bool hasPortalIC = false;
  WaylandWarnings waylandWarnings(deps);

  EXPECT_CALL(*deps, showNoEiSupport(nullptr)).Times(1);

  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
}

TEST(WaylandWarningsTests, showOnce_serverNoPortal_showNoPortalWarning) {
  const auto deps = std::make_shared<MockDeps>();
  const bool hasEi = true;
  const bool hasPortal = false;
  const bool hasPortalIC = false;
  WaylandWarnings waylandWarnings(deps);

  EXPECT_CALL(*deps, showNoPortalSupport(nullptr)).Times(1);

  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
}

TEST(
    WaylandWarningsTests,
    showOnce_serverHasEiAndNoPortalIc_showPortalIcWarning) {
  const auto deps = std::make_shared<MockDeps>();
  const bool hasEi = true;
  const bool hasPortal = true;
  const bool hasPortalIC = false;
  WaylandWarnings waylandWarnings(deps);

  EXPECT_CALL(*deps, showNoPortalInputCapture(nullptr)).Times(1);

  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
}

TEST(
    WaylandWarningsTests,
    showOnce_serverHasEiAndPortalIc_showExperimentalOnly) {
  const auto deps = std::make_shared<MockDeps>();
  const bool hasEi = true;
  const bool hasPortal = true;
  const bool hasPortalIC = true;
  WaylandWarnings waylandWarnings(deps);

  EXPECT_CALL(*deps, showWaylandExperimental(nullptr)).Times(1);

  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
}

TEST(WaylandWarningsTests, showOnce_failureCalledTwice_messageOnlyShownOnce) {
  const auto deps = std::make_shared<MockDeps>();
  const bool hasEi = false;
  const bool hasPortal = false;
  const bool hasPortalIC = false;
  WaylandWarnings waylandWarnings(deps);

  EXPECT_CALL(*deps, showNoEiSupport(nullptr)).Times(1);

  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
}

TEST(WaylandWarningsTests, showOnce_successCalledTwice_messageOnlyShownOnce) {
  const auto deps = std::make_shared<MockDeps>();
  const bool hasEi = true;
  const bool hasPortal = true;
  const bool hasPortalIC = true;
  WaylandWarnings waylandWarnings(deps);

  EXPECT_CALL(*deps, showWaylandExperimental(nullptr)).Times(1);

  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
  waylandWarnings.showOnce(
      nullptr, CoreProcess::Mode::Server, hasEi, hasPortal, hasPortalIC);
}
