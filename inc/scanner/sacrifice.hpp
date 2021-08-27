#ifndef __SACRIFICE_HPP__
#define __SACRIFICE_HPP__

// sizeof(entity_t) <= 0x548
namespace sacrifice_offsets
{
  const uint32_t this_addr = 0x18;
  const uint32_t x = 0x1B8;
  const uint32_t y = 0x1BC;
  const uint32_t z = 0x1C0;
  const uint32_t owning_wizard = 0x1C4;
  const uint32_t next_entity = 0x3cc;
  const uint32_t wizard_name = 0x1440;

  const uint32_t creature_list = 0xDD5D0;
  const uint32_t wizard_list = 0xDD5D4;
  const uint32_t structure_list = 0xDD5D8;
}

#endif
