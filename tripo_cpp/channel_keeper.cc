#include "channel_keeper.h"

namespace foundersc {
namespace tripo {

int ChannelKeeper::Initialize() {
  service_discoverer_->Initialize();

  if (no_arg_selector_factory_) {
    auto watcher = std::bind(&ChannelKeeper::UpdateNoArgSelector, this, std::placeholders::_1);
    service_discoverer_->AddServiceNodesWatcher(watcher);
  } else if (by_id_selector_factory_) {
    auto watcher = std::bind(&ChannelKeeper::UpdateByIdSelector, this, std::placeholders::_1);
    service_discoverer_->AddServiceNodesWatcher(watcher);
  } else if (by_key_selector_factory_) {
    auto watcher = std::bind(&ChannelKeeper::UpdateByKeySelector, this, std::placeholders::_1);
    service_discoverer_->AddServiceNodesWatcher(watcher);
  } else {
    return 1;
  }
  return 0;
}

}
}

