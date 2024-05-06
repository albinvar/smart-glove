<?php

namespace App\Livewire;

use App\Models\Appliance;
use Livewire\Component;

class Home extends Component
{
    public function render()
    {
        // get all appliances
        $appliance1= Appliance::find(1);
        $appliance2= Appliance::find(2);

        return view('livewire.home', compact('appliance1', 'appliance2'));
    }
}
