from django.shortcuts import render
from .models import Order, OrderItem
from .forms import OrderCreateForm
from cart.cart import Cart
from django.contrib.auth.decorators import login_required
# from .tasks import order_created

def order_create(request):
    cart = Cart(request)
    if request.method == 'POST':
        form = OrderCreateForm(request.POST)
        if form.is_valid():
            form.instance.owner = request.user
            order = form.save()
            for item in cart:
                # Order.objects.create(owner = request.user,
                #                      addr_x = form.cleaned_data.get('addr_x'),
                #                      addr_y = form.cleaned_data.get('addr_y'), 
                #                      ups_id = form.cleaned_data.get('ups_id'),
                #                         product=item['product'],
                #                         price=item['price'],
                #                         product_cnt=item['quantity'],
                #                         )
                OrderItem.objects.create(order=order,
                                        product=item['product'],
                                        price=item['price'],
                                        quantity=item['quantity'])
            # clear the cart
            cart.clear()
            # launch asynchronous task
            #order_created.delay(order.id)
            return render(request,
                          'order/created.html',
                          {'order': order})
    else:
        form = OrderCreateForm()
    return render(request,
                  'order/create.html',
                  {'cart': cart, 'form': form})
    
@login_required
def viewOrder(request):
    order_list = Order.objects.filter(owner=request.user)
    context = {
        'order_list':order_list,
    }
    return render(request, 'order/orders.html', context)
